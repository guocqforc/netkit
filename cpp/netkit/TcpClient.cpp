//
// Created by dantezhu on 14-8-13.
// Copyright (c) 2014 dantezhu. All rights reserved.
//

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)

#include <winsock2.h>

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#endif

#include "TcpClient.h"

namespace netkit {

TcpClient::TcpClient(const std::string &host, short port, double timeout) {
    m_host = host;
    m_port = port;
    m_timeout = timeout;
    m_stream = NULL;
}

TcpClient::~TcpClient() {
    if (m_stream) {
        delete m_stream;
        m_stream = NULL;
    }
}

int TcpClient::connectServer() {

    struct sockaddr_in serv_addr;
    struct in_addr ip_addr;
    ip_addr.s_addr = inet_addr(m_host.c_str());

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(m_port);
    serv_addr.sin_addr = ip_addr;

    SocketType sockFd;

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if ( sockFd < 0 )
    {
        return -1;
    }

    if (m_timeout > 0) {
        struct timeval timeout={
                (int)m_timeout,
                (int)((m_timeout - (int)m_timeout) * 1000000)
        };

        setsockopt(sockFd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));
    }

    int ret = connect(sockFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        CLOSE_SOCKET(sockFd);
        return -2;
    }

    m_stream = new Stream(sockFd);

    return 0;
}

// 0 为成功
int TcpClient::read(IBox* box) {
    if (!m_stream) {
        return -1;
    }

    return m_stream->read(box);
}

// 0 为成功
int TcpClient::write(IBox* box) {
    if (!m_stream) {
        return -1;
    }

    return m_stream->write(box);
}

int TcpClient::write(const char* buf, int bufLen) {
    if (!m_stream) {
        return -1;
    }

    return m_stream->write(buf, bufLen);
}

void TcpClient::closeStream() {
    if (!m_stream) {
        return;
    }

    return m_stream->closeStream();
}

bool TcpClient::isClosed() {
    if (!m_stream) {
        return true;
    }

    return m_stream->isClosed();
}

}
