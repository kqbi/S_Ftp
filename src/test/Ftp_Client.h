//
// Created by kqbi on 2020/6/17.
//

#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "S_FtpClientAPI.h"
#include <string>

class Ftp_Client {
public:
    Ftp_Client();

    ~Ftp_Client();

    bool Login(const std::string &ip, uint16_t port, const std::string &userName, const std::string &pwd);

    bool UploadPicFile(const std::string &strLocalFile, const std::string &strRemoteFile);

    bool UploadBuffer(const std::string &buffer, const std::string &strRemoteFile);

public:
    ftp_client _ftpClient;

    uint64_t _sessionId;

    uint64_t _handleId;
};


#endif //FTP_CLIENT_H
