/********************************************************************
    Rhapsody	: 8.4
    Login		: Administrator
    Component	: DefaultComponent
    Configuration 	: S_HTTP_Config
    Model Element	: S_HTTP_Config
//!	Generated Date	: Fri, 14, Jun 2019  
    File Path	: ../../src/MainDefaultComponent.cpp
*********************************************************************/

#include <oxf.h>
#include <iostream>
#include <signal.h>
#include <shellapi.h>
#include "Ftp_Client.h"
#include "MiniDumper.h"
static bool finished = false;
static int cur_tid = 0;
static void
signalHandler(int signo) {
    std::cerr << "Shutting down" << std::endl;
    finished = true;
}

int main(int argc, char *argv[]) {
    int status = 0;
    CMiniDumper _miniDumper( true );
    if (OXF::Instance().Initialize()) {
#ifndef _WIN32
        if ( signal( SIGPIPE, SIG_IGN) == SIG_ERR)
        {
            std::cerr << "Couldn't install signal handler for SIGPIPE" << std::endl;
            exit(-1);
        }
        //        if ( signal( SIGHUP,signalHandler ) == SIG_ERR )
        //        {
        //            std::cerr << "Couldn't install signal handler for SIGHUP" << std::endl;
        //            exit( -1 );
        //        }
#endif

        if (signal(SIGINT, signalHandler) == SIG_ERR) {
            std::cerr << "Couldn't install signal handler for SIGINT" << std::endl;
            exit(-1);
        }

        if (signal(SIGTERM, signalHandler) == SIG_ERR) {
            std::cerr << "Couldn't install signal handler for SIGTERM" << std::endl;
            exit(-1);
        }

        Ftp_Client *client = new Ftp_Client();

        if (client->Login("124.112.209.60", 21, "ftpUser", ".qbzx@110.com")) {
            printf("Login success\n");
        } else {
            printf("Login failed\n");
        }

        if (client->UploadPicFile("D:/test/untitled/untitled.pro", "/sdtpic/sss")) {
            printf("UploadPicFile success\n");
        } else {
            printf("UploadPicFile failed\n");
        }

        if (client->UploadBuffer("D:/test/untitled/untitled.pro", "/sdtpic/1111")) {
            printf("UploadPicFile success\n");
        } else {
            printf("UploadPicFile failed\n");
        }
        while (!finished) {
           Sleep(2000);
        }
        delete client;
        ::exit(status);
        status = 0;
    } else {
        status = 1;
    }
    return status;
}

/*********************************************************************
    File Path	: ../../src/MainDefaultComponent.cpp
*********************************************************************/
