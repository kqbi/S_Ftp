//
// Created by kqbi on 2020/5/29.
//

#ifndef S_WEBSOCKET_UTIL_H
#define S_WEBSOCKET_UTIL_H

#include <string>
#include <boost/utility/string_view.hpp>

std::string getNewConnectionId();

std::string FindField(const char *buf, const char *start, const char *end, int bufSize = 0);

std::string base64_encode(std::uint8_t const *data, std::size_t len);

std::string base64_encode(boost::string_view s);

std::string base64_decode(boost::string_view data);

std::string exePath();

std::string exeDir();

std::string exeName();

#endif //S_WEBSOCKET_UTIL_H
