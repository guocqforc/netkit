//
// Created by dantezhu on 14-8-13.
// Copyright (c) 2014 dantezhu. All rights reserved.
//


#ifndef TCPCLIENT_H_20140814144656
#define TCPCLIENT_H_20140814144656


#include <iostream>
#include "IBox.h"
#include "Stream.h"

namespace netkit {

class TcpClient {
public:
    // timeout <=0 代表永不超时
    TcpClient(const std::string &host, int port, double timeout);
    virtual ~TcpClient();

    int connect();

    // 0 为成功
    int read(IBox* box);

    // 0 为成功
    int write(IBox* box);
    int write(const char* buf, int bufLen);

    void shutdown(int how);

    void close();

    bool isClosed();

    void setTcpNoDelay(bool tcpNoDelay);
    bool getTcpNoDelay();

private:

    // 设置阻塞/非阻塞socket
    void _setBlockSocket(netkit::SocketType sockFd, bool block);

    // 设置 TCP_NODELAY
    int _setTcpNoDelay(netkit::SocketType sockFd);

    // 通过FQDN创建socket。同时支持IPv4/IPv6
    int _createUnitySocket(std::string host, int port, netkit::SocketType &resultSock, std::string &address);

    // 创建IPv4 socket
    int _createV4Socket(std::string host, int port, netkit::SocketType &resultSock, std::string &address);

    // 创建IPv6 socket
    int _createV6Socket(std::string host, int port, netkit::SocketType &resultSock, std::string &address);

    // 自定义连接，内部可能使用以下3种连接
    int _customConnect(std::string host, int port, double timeout,
                       netkit::SocketType &resultSock);

    // 同步连接
    int _blockConnect(std::string host, int port,
                       netkit::SocketType &resultSock);
    
    // 通过select异步连接
    int _selectConnect(std::string host, int port, double timeout,
                       netkit::SocketType &resultSock);

#if !defined(_WIN32) && !(defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
    // 通过poll异步连接
    int _pollConnect(std::string host, int port, double timeout,
                       netkit::SocketType &resultSock);
#endif


private:
    std::string m_host;
    int m_port;
    double m_timeout;

    bool m_tcpNoDelay;

    Stream *m_stream;
};

}

#endif //__TcpClient_H_
