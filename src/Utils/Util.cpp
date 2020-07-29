//
// Created by kqbi on 2020/5/29.
//

#include "Util.h"
#include <boost/beast/core/detail/base64.hpp>

#if defined(_WIN32)
#include <windows.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#elif defined(__linux__)
#include <uuid/uuid.h>
#endif

std::string getNewConnectionId() {
    //#[ operation getNewConnectionId()
#ifdef __linux__
    uuid_t uuid;
    char str[36];
    uuid_generate(uuid);
    uuid_unparse(uuid, str);
    return str;
#elif _WIN32
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return boost::uuids::to_string(uuid);
#endif
    //#]
}

std::string FindField(const char *buf, const char *start, const char *end, int bufSize) {
    if (bufSize <= 0) {
        bufSize = strlen(buf);
    }
    const char *msg_start = buf, *msg_end = buf + bufSize;
    int len = 0;
    if (start != NULL) {
        len = strlen(start);
        msg_start = strstr(buf, start);
    }
    if (msg_start == NULL) {
        return "";
    }
    msg_start += len;
    if (end != NULL) {
        msg_end = strstr(msg_start, end);
        if (msg_end == NULL) {
            return "";
        }
    }
    return std::string(msg_start, msg_end);
}

std::string base64_encode(std::uint8_t const *data, std::size_t len) {
    std::string dest;
    dest.resize(boost::beast::detail::base64::encoded_size(len));
    dest.resize(boost::beast::detail::base64::encode(&dest[0], data, len));
    return dest;
}

std::string base64_encode(boost::string_view s) {
    return base64_encode(reinterpret_cast <
                                 std::uint8_t const *> (s.data()), s.size());
}

std::string base64_decode(boost::string_view data) {
    std::string dest;
    dest.resize(boost::beast::detail::base64::decoded_size(data.size()));
    auto const result = boost::beast::detail::base64::decode(
            &dest[0], data.data(), data.size());
    dest.resize(result.first);
    return dest;
}

std::string exePath() {
    char buffer[1024 * 2 + 1] = { 0 };
    int n = -1;
#if defined(_WIN32)
    n = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
#elif defined(__MACH__) || defined(__APPLE__)
    n = sizeof(buffer);
	if (uv_exepath(buffer, &n) != 0) {
		n = -1;
	}
#elif defined(__linux__)
	n = readlink("/proc/self/exe", buffer, sizeof(buffer));
#endif

    std::string filePath;
    if (n <= 0) {
        filePath = "./";
    }
    else {
        filePath = buffer;
    }

#if defined(_WIN32)
    for (auto &ch : filePath) {
        if (ch == '\\') {
            ch = '/';
        }
    }
#endif //defined(_WIN32)

    return filePath;
}

std::string exeDir() {
    auto path = exePath();
    return path.substr(0, path.rfind('/') + 1);
}

std::string exeName() {
    auto path = exePath();
    return path.substr(path.rfind('/') + 1);
}