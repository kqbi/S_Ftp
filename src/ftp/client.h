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

#ifndef FTP_CLIENT_HPP
#define FTP_CLIENT_HPP

#include "detail/control_connection.hpp"
#include "detail/data_connection.hpp"
#include <string>
#include <list>

namespace S_Ftp {

enum class command_result
{
    ok = 0,
    not_ok,
    error
};

class client
{
public:
    client(boost::asio::io_context &ioc);

    client(const client &) = delete;

    client & operator=(const client &) = delete;

    command_result open(const std::string & hostname, uint16_t port = 21);

    bool is_open() const;

    command_result login(const std::string & username, const std::string & password);

    command_result cd(const std::string & remote_directory);

    command_result ls(const std::string& remote_directory= "");

    command_result uploadFile(const std::string & local_file, const std::string & remote_file);

    command_result uploadBuffer(const std::string &buffer, const std::string & remote_file);

    command_result download(const std::string & remote_file, const std::string & local_file);

    command_result pwd();

    command_result mkdir(const std::string & directory_name);

    command_result rmdir(const std::string & directory_name);

    command_result rm(const std::string & remote_file);

    command_result binary();

    command_result size(const std::string & remote_file);

    command_result stat(const std::string &remote_file = "");

    command_result system();

    command_result noop();

    command_result close();

    class event_observer
    {
    public:
        virtual void on_reply(const std::string & reply) = 0;

        virtual void on_error(const std::string & error) = 0;

        virtual ~event_observer() = default;
    };

    void subscribe(event_observer *observer);

    void unsubscribe(event_observer *observer);

private:
    command_result create_data_connection(std::unique_ptr<data_connection> &dataConnection);

    static bool try_parse_server_port(const std::string & epsv_reply, uint16_t & port);

    void handle_connection_exception(const std::exception & ex);

    void report_reply(const std::string & reply);

    void report_reply(const reply_t & reply);

    void report_error(const std::string & error);

    control_connection control_connection_;
    std::list<event_observer *> observers_;
    std::array<char, 8192> buffer_;
    boost::asio::io_context &_ioc;
};

} // namespace ftp
#endif //FTP_CLIENT_HPP
