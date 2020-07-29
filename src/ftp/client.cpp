/*
 * MIT License
 *
 * Copyright (c) 2019 Denis Kovalchuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "client.h"
#include <fstream>
#include <boost/lexical_cast.hpp>

namespace S_Ftp {

    client::client(boost::asio::io_context &ioc) : _ioc(ioc),control_connection_(ioc) {

    }

    command_result client::open(const std::string &hostname, uint16_t port) {
        try {
            control_connection_.open(hostname, port);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    bool client::is_open() const {
        return control_connection_.is_open();
    }

    command_result client::login(const std::string &username, const std::string &password) {
        try {
            control_connection_.send("USER " + username);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.status_code == 331) {
                /* 331 User name okay, need password. */
                control_connection_.send("PASS " + password);

                reply = control_connection_.recv();

                report_reply(reply);
            } else if (reply.status_code == 332) {
                /* 332 Need account for login.
                 * Sorry, we don't support ACCT command.
                 */
            }

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::cd(const std::string &remote_directory) {
        try {
            control_connection_.send("CWD " + remote_directory);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::ls(const std::string &remote_directory) {
        std::string command;

        if (!remote_directory.empty()) {
            command = "LIST " + remote_directory;
        } else {
            command = "LIST";
        }

        try {
            std::unique_ptr<data_connection> data_connection = 0;
            auto result = create_data_connection(data_connection);

            if (result != command_result::ok) {
                return result;
            }

            control_connection_.send(command);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            }

            report_reply(data_connection->recv());

            /* Don't keep the data connection. */
            data_connection->close();

            reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::uploadFile(const std::string &local_file, const std::string &remote_file) {
        std::ifstream file(local_file, std::ios_base::binary);

        if (!file) {
            std::string error_msg = "Cannot open file:" + local_file;
            report_error(error_msg);
            return command_result::error;
        }

        try {
            std::unique_ptr<data_connection> data_connection = 0;
            auto result = create_data_connection(data_connection);

            if (result != command_result::ok) {
                return result;
            }

            control_connection_.send("STOR " + remote_file);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            }

            /* Start file transfer. */
            for (;;) {
                file.read(buffer_.data(), buffer_.size());

                if (file.fail() && !file.eof()) {
                    report_error("Cannot read data from file.");
                    return command_result::error;
                }

                data_connection->send(buffer_.data(), file.gcount());

                if (file.eof())
                    break;
            }

            /* Don't keep the data connection. */
            data_connection->close();

            reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::uploadBuffer(const std::string &buffer, const std::string &remote_file) {
        try {
            std::unique_ptr<data_connection> data_connection = 0;
            auto result = create_data_connection(data_connection);

            if (result != command_result::ok) {
                return result;
            }

            control_connection_.send("STOR " + remote_file);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            }

            data_connection->send(boost::asio::buffer(buffer.data(), buffer.size()));

            /* Don't keep the data connection. */
            data_connection->close();

            reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::download(const std::string &remote_file, const std::string &local_file) {
        std::ofstream file(local_file, std::ios_base::binary);

        if (!file) {
            std::string error_msg = "Cannot create file:" + local_file;
            report_error(error_msg);
            return command_result::error;
        }

        try {
            std::unique_ptr<data_connection> data_connection = 0;
            auto result = create_data_connection(data_connection);

            if (result != command_result::ok) {
                return result;
            }

            control_connection_.send("RETR " + remote_file);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            }

            /* Start file transfer. */
            for (;;) {
                size_t size = data_connection->recv(buffer_.data(), buffer_.size());

                if (size == 0)
                    break;

                file.write(buffer_.data(), size);

                if (file.fail()) {
                    report_error("Cannot write data to file.");
                    return command_result::error;
                }
            }

            /* Don't keep the data connection. */
            data_connection->close();

            reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::pwd() {
        try {
            control_connection_.send("PWD");

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::mkdir(const std::string &directory_name) {
        try {
            control_connection_.send("MKD " + directory_name);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::rmdir(const std::string &directory_name) {
        try {
            control_connection_.send("RMD " + directory_name);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::rm(const std::string &remote_file) {
        try {
            control_connection_.send("DELE " + remote_file);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::binary() {
        try {
            control_connection_.send("TYPE I");

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::size(const std::string &remote_file) {
        try {
            control_connection_.send("SIZE " + remote_file);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::stat(const std::string &remote_file) {
        std::string command;

        if (!remote_file.empty()) {
            command = "STAT " + remote_file;
        } else {
            command = "STAT";
        }

        try {
            control_connection_.send(command);

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::system() {
        try {
            control_connection_.send("SYST");

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::noop() {
        try {
            control_connection_.send("NOOP");

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::close() {
        try {
            control_connection_.send("QUIT");

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            control_connection_.close();

            if (reply.is_negative()) {
                return command_result::not_ok;
            } else {
                return command_result::ok;
            }
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

    command_result client::create_data_connection(std::unique_ptr<data_connection> &dataConnection) {
        try {
            control_connection_.send("EPSV");

            reply_t reply = control_connection_.recv();

            report_reply(reply);

            if (reply.is_negative()) {
                return command_result::not_ok;
            }

            uint16_t port = 0;
            if (!try_parse_server_port(reply.status_line, port)) {
                std::string error_msg = "Cannot parse server port from " + reply.status_line;
                report_error(error_msg);
                return command_result::error;
            }

            std::unique_ptr<data_connection> connection = std::make_unique<data_connection>(_ioc);

            connection->open(control_connection_.ip(), port);
            dataConnection = std::move(connection);
            return command_result::ok;
        }
        catch (const std::exception &ex) {
            handle_connection_exception(ex);
            return command_result::error;
        }
    }

/* The text returned in response to the EPSV command MUST be:
 *
 *     <text indicating server is entering extended passive mode> \
 *       (<d><d><d><tcp-port><d>)
 *
 * The first two fields contained in the parenthesis MUST be blank.  The
 * third field MUST be the string representation of the TCP port number
 * on which the server is listening for a data connection.  The network
 * protocol used by the data connection will be the same network
 * protocol used by the control connection.  In addition, the network
 * address used to establish the data connection will be the same
 * network address used for the control connection.  An example response
 * string follows:
 *
 *     Entering Extended Passive Mode (|||6446|)
 *
 * RFC 2428: https://tools.ietf.org/html/rfc2428
 */
    bool client::try_parse_server_port(const std::string &epsv_reply, uint16_t &port) {
        size_t begin = epsv_reply.find('|');
        if (begin == std::string::npos) {
            return false;
        }

        /* Skip '|||' characters. */
        begin += 3;
        if (begin >= epsv_reply.size()) {
            return false;
        }

        size_t end = epsv_reply.rfind('|');
        if (end == std::string::npos) {
            return false;
        }

        if (end <= begin) {
            return false;
        }

        std::string port_str = epsv_reply.substr(begin, end - begin);

        return boost::conversion::try_lexical_convert(port_str, port);
    }

    void client::handle_connection_exception(const std::exception &ex) {
        std::string error_msg = ex.what();

        report_error(error_msg);

        control_connection_.reset();
    }

    void client::subscribe(event_observer *observer) {
        observers_.push_back(observer);
    }

    void client::unsubscribe(event_observer *observer) {
        observers_.remove(observer);
    }

    void client::report_reply(const std::string &reply) {
        for (const auto &observer : observers_) {
            if (observer)
                observer->on_reply(reply);
        }
    }

    void client::report_reply(const reply_t &reply) {
        for (const auto &observer : observers_) {
            if (observer)
                observer->on_reply(reply.status_line);
        }
    }

    void client::report_error(const std::string &error) {
        for (const auto &observer : observers_) {
            if (observer)
                observer->on_error(error);
        }
    }

} // namespace ftp
