//
// Created by dantezhu on 14-7-26.
// Copyright (c) 2014 dantezhu. All rights reserved.
//


#ifndef STREAM_H_20140814144646
#define STREAM_H_20140814144646


#include <iostream>
#include "IBox.h"

namespace netkit {

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
#include <winsock2.h>
typedef SOCKET SocketType;

#define CLOSE_SOCKET    closesocket

#else
typedef int SocketType;

#define CLOSE_SOCKET    ::close
#endif

class Stream {
public:
    // 初始化时分配的buf大小
    static const int INIT_READ_BUFFER_SIZE = 4 * 1024;
    static const int RET_RECV_TIMEOUT = -100;

private:
    int m_sockFd;
    // 实际长度
    int m_bufferedLength;
    int m_readBufferMaxsize;
    std::string m_readBuffer;

private:
    void _init(SocketType sockFd, int initReadBufferSize, int readBufferMaxsize);

public:
    Stream(SocketType sockFd);
    Stream(SocketType sockFd, int initReadBufferSize);
    Stream(SocketType sockFd, int initReadBufferSize, int readBufferMaxsize);
    virtual ~Stream();

    void setSockFd(SocketType sockFd);

    // 0 为成功
    int read(IBox* box);

    // 0 为成功
    int write(IBox* box);
    int write(const char* buf, int bufLen);

    // 关闭输入输出
    // 可以触发read报错
    void shutdown(int how);

    void close();

    bool isClosed();

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)

    //这两个函数只有在windows下面才用
    //初始化网络模块，一定不能忘，在main函数里面调用一次就可以了。
    static int startupSocket()
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int ret;
        //WinSock初始化
        wVersionRequested = MAKEWORD(1, 1); //希望使用的WinSock DLL的版本
        ret = WSAStartup(wVersionRequested, &wsaData);
        if(ret!=0)
        {
            //printf("WSAStartup() failed!\n");
            return -1;
        }
        //确认WinSock DLL支持版本2.2
        if(LOBYTE(wsaData.wVersion)!=1 || HIBYTE(wsaData.wVersion)!=1)
        {
            WSACleanup();
            //printf("Invalid WinSock version!\n");
            return -2;
        }

        return 0;
    }
    //这个在游戏退出的时候调用就可以了
    static void cleanupSocket()
    {
        WSACleanup();
    }

#endif
};

}


#endif //__Stream_H_
