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

#ifndef FTP_DATA_CONNECTION_HPP
#define FTP_DATA_CONNECTION_HPP

#include <boost/asio.hpp>
#include <queue>

namespace S_Ftp {

    class data_connection {
    public:
        data_connection(boost::asio::io_context &ioc);

        data_connection(const data_connection &) = delete;

        data_connection &operator=(const data_connection &) = delete;

        void open(const std::string &ip, uint16_t port);

        bool is_open() const;

        void close();

        void send(const void *buf, std::size_t size);

        void asyncWrite(boost::asio::const_buffer &msg);

        void handleWrite(boost::system::error_code ec, std::size_t bytesTransferred);

        void send(boost::asio::const_buffer &msg);

        std::size_t recv(void *buf, std::size_t max_size);

        std::string recv();

    private:
        boost::asio::ip::tcp::socket _socket;
        boost::asio::io_context::strand _strand;
        std::queue<boost::asio::const_buffer> _queue;
    };

} // namespace ftp::detail
#endif //FTP_DATA_CONNECTION_HPP
