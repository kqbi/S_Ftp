//
// Created by kqbi on 2020/6/16.
//

#include "FtpClient_Service.h"
#include <assert.h>
#include <S_FtpClientAPI.h>
#include "Logger.h"

using namespace S_Ftp;

ftp_client S_FTP_CLIENT_CALL S_FtpClient_Create(boost::asio::io_context &ioc) {
    FtpClient_Service *service = new FtpClient_Service(ioc);
    Logger::Instance().Init("janus.log", 0, 0, 50, 5);
    return (ftp_client) service;
}

void S_FTP_CLIENT_CALL S_FtpClient_Release(ftp_client ctx) {
    assert(ctx);
    delete (FtpClient_Service *) ctx;
}

bool S_FTP_CLIENT_CALL S_FtpClient_Login(ftp_client ctx, const std::string &ip, uint16_t port, const std::string &userName, const std::string &pwd) {
    assert(ctx);
    return ((FtpClient_Service *) ctx)->Login(ip, port,userName, pwd);
}

bool S_FTP_CLIENT_CALL S_FtpClient_UploadPicFile(ftp_client ctx, const std::string& strLocalFile, const std::string& strRemoteFile) {
    assert(ctx);
    return ((FtpClient_Service *) ctx)->UploadPicFile(strLocalFile, strRemoteFile);
}

bool S_FTP_CLIENT_CALL S_FtpClient_UploadBuffer(ftp_client ctx, const std::string& buffer, const std::string& strRemoteFile) {
    assert(ctx);
    return ((FtpClient_Service *) ctx)->UploadBuffer(buffer, strRemoteFile);
}