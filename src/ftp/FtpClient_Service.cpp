//
// Created by kqbi on 2020/7/28.
//

#include "FtpClient_Service.h"

namespace S_Ftp {
    FtpClient_Service::FtpClient_Service(boost::asio::io_context &ioc) : _client(0) {
        _client = new client(ioc);
    }

    FtpClient_Service::~FtpClient_Service() {
        if (_client) {
            delete _client;
        }
    }

    bool FtpClient_Service::Login(const std::string &ip, uint16_t port, const std::string &userName, const std::string &pwd) {
        bool ret = false;
        command_result result = _client->open(ip, port);
        if (result == command_result::ok) {
            result = _client->login(userName, pwd);
            if (result == command_result::ok) {
                ret = true;
            }
        } else {
            printf("open failed\n");
        }
        return ret;
    }

    bool FtpClient_Service::UploadPicFile(const std::string &strLocalFile, const std::string &strRemoteFile,
                                          const bool bCreateDir) {
        bool ret = false;
        command_result result = _client->uploadFile(strLocalFile, strRemoteFile);
        if (result == command_result::ok) {
            ret = true;
        }
        return ret;
    }

    bool FtpClient_Service::UploadBuffer(const std::string &buffer, const std::string &strRemoteFile,
                                          const bool bCreateDir) {
        bool ret = false;
        command_result result = _client->uploadBuffer(buffer, strRemoteFile);
        if (result == command_result::ok) {
            ret = true;
        }
        return ret;
    }
}