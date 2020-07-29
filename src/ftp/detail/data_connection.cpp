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

#include "data_connection.hpp"

namespace S_Ftp {

    data_connection::data_connection(boost::asio::io_context &ioc)
            : _socket(ioc),
              _strand(ioc) {
    }

    void data_connection::open(const std::string &ip, std::uint16_t port) {
        boost::system::error_code ec;

        boost::asio::ip::address address = boost::asio::ip::address::from_string(ip, ec);

        if (ec) {
            throw std::exception(ec.message().c_str());
        }

        boost::asio::ip::tcp::endpoint remote_endpoint(address, port);
        _socket.connect(remote_endpoint, ec);

        if (ec) {
            boost::system::error_code ignored;

            /* If the connect fails, and the socket was automatically opened,
             * the socket is not returned to the closed state.
             *
             * https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/reference/basic_stream_socket/connect/overload2.html
             */
            _socket.close(ignored);

            throw std::exception(ec.message().c_str());
        }
    }

    bool data_connection::is_open() const {
        return _socket.is_open();
    }

    void data_connection::close() {
        boost::system::error_code ec;

        _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

        if (ec == boost::asio::error::not_connected) {
            /* Ignore 'not_connected' error. We could get ENOTCONN if an server side
             * has already closed the data connection. This suits us, just close
             * the socket.
             */
        } else if (ec) {
            throw std::exception(ec.message().c_str());
        }

        _socket.close(ec);

        if (ec) {
            throw std::exception(ec.message().c_str());
        }
    }

    void data_connection::send(const void *buf, size_t size) {
        boost::system::error_code ec;

        boost::asio::write(_socket, boost::asio::buffer(buf, size), ec);

        if (ec) {
            throw std::exception(ec.message().c_str());
        }
    }

    void data_connection::asyncWrite(boost::asio::const_buffer &msg) {
        _queue.push(msg);

        // Are we already writing?
        if (_queue.size() > 1)
            return;

        // We are not currently writing, so send this immediately
        if (_socket.is_open()) {
            boost::asio::async_write(_socket,
                                     _queue.front(),
                                     _strand.wrap(std::bind(
                                             &data_connection::handleWrite,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2)));
        }
    }

    void data_connection::handleWrite(boost::system::error_code ec, std::size_t bytesTransferred) {
        if (ec) {
            //_queue.clear();
            if (ec == boost::asio::error::operation_aborted)
                return;
        } else {
            _queue.pop();

            // Send the next message if any
            if (!_queue.empty() && _socket.is_open()) {
                boost::asio::async_write(_socket,
                                         _queue.front(),
                                         _strand.wrap(std::bind(
                                                 &data_connection::handleWrite,
                                                 this,
                                                 std::placeholders::_1,
                                                 std::placeholders::_2)));
            }
        }
    }

    void data_connection::send(boost::asio::const_buffer &msg) {
        boost::system::error_code ec;

        boost::asio::write(_socket, msg, ec);

        if (ec) {
            throw std::exception(ec.message().c_str());
        }
    }

    size_t data_connection::recv(void *buf, size_t max_size) {
        boost::system::error_code ec;

        size_t size = _socket.read_some(boost::asio::buffer(buf, max_size), ec);

        if (ec == boost::asio::error::eof) {
            /* Ignore eof. */
        } else if (ec) {
            throw std::exception(ec.message().c_str());
        }

        return size;
    }

    std::string data_connection::recv() {
        boost::system::error_code ec;
        std::string reply;

        boost::asio::read(_socket, boost::asio::dynamic_buffer(reply), ec);

        if (ec == boost::asio::error::eof) {
            /* Ignore eof. */
        } else if (ec) {
            throw std::exception(ec.message().c_str());
        }

        return reply;
    }

} // namespace ftp::detail
