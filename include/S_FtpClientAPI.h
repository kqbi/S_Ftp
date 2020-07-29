//
// Created by kqbi on 2020/6/16.
//

#ifndef S_FTPCLIENTAPI_H
#define S_FTPCLIENTAPI_H

#include <string>
#include <boost/asio/io_context.hpp>

#if defined( _WIN32 ) || defined( __MINGW32__ )
#   if defined( S_FTP_CLIENT_EXPORTS )
#       define S_FTP_CLIENT_EXPORT __declspec(dllexport)
#       define S_FTP_CLIENT_CALL __stdcall
#   elif defined( S_FTP_CLIENT_USE_DLL_IMPORT ) || !defined( S_FTP_CLIENT_USE_STATIC_LIB )
#       define S_FTP_CLIENT_EXPORT __declspec(dllimport)
#       define S_FTP_CLIENT_CALL __stdcall
#   else
#       define S_FTP_CLIENT_EXPORT
#       define S_FTP_CLIENT_CALL
#   endif
#elif defined(__linux__) || defined(__APPLE__) //linux
#   define S_FTP_CLIENT_EXPORT
#   define S_FTP_CLIENT_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef void *ftp_client;

S_FTP_CLIENT_EXPORT ftp_client S_FTP_CLIENT_CALL S_FtpClient_Create(boost::asio::io_context &ioc);

S_FTP_CLIENT_EXPORT void S_FTP_CLIENT_CALL S_FtpClient_Release(ftp_client ctx);

S_FTP_CLIENT_EXPORT bool S_FTP_CLIENT_CALL S_FtpClient_Login(ftp_client ctx, const std::string &ip, uint16_t port, const std::string &userName, const std::string &pwd);

S_FTP_CLIENT_EXPORT bool S_FTP_CLIENT_CALL S_FtpClient_UploadPicFile(ftp_client ctx, const std::string& strLocalFile, const std::string& strRemoteFile);

S_FTP_CLIENT_EXPORT bool S_FTP_CLIENT_CALL S_FtpClient_UploadBuffer(ftp_client ctx, const std::string& buffer, const std::string& strRemoteFile);

#ifdef __cplusplus
}
#endif

#endif //S_FTPCLIENTAPI_H
