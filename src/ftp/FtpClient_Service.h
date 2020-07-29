//
// Created by kqbi on 2020/7/28.
//

#ifndef S_FTP_FTPCLIENT_SERVICE_H
#define S_FTP_FTPCLIENT_SERVICE_H

#include "client.h"
namespace S_Ftp {
    class FtpClient_Service {
    public:
        FtpClient_Service(boost::asio::io_context &ioc);

        ~FtpClient_Service();

        bool Login(const std::string &ip, uint16_t port, const std::string &userName, const std::string &pwd);

        bool UploadPicFile(const std::string& strLocalFile, const std::string& strRemoteFile, const bool bCreateDir = false);

        bool UploadBuffer(const std::string &buffer, const std::string &strRemoteFile, const bool bCreateDir = false);

        client *_client;

    };
}

#endif //S_FTP_FTPCLIENT_SERVICE_H
