//
// Created by kqbi on 2020/6/17.
//

#include "Ftp_Client.h"
#include "OXFWorkPoller.h"

Ftp_Client::Ftp_Client() : _ftpClient(0), _sessionId(0), _handleId(0) {
    _ftpClient = S_FtpClient_Create(OXFWorkPollerPool::Instance()._ioc);
}

Ftp_Client::~Ftp_Client() {
    S_FtpClient_Release(_ftpClient);
}

bool Ftp_Client::Login(const std::string &ip, uint16_t port, const std::string &userName, const std::string &pwd) {
    return S_FtpClient_Login(_ftpClient, ip, port, userName, pwd);
}

bool Ftp_Client::UploadPicFile(const std::string &strLocalFile, const std::string &strRemoteFile) {
    return S_FtpClient_UploadPicFile(_ftpClient, strLocalFile, strRemoteFile);
}

bool Ftp_Client::UploadBuffer(const std::string &buffer, const std::string &strRemoteFile) {
    return S_FtpClient_UploadBuffer(_ftpClient, buffer, strRemoteFile);
}
