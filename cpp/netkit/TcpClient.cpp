//
// Created by dantezhu on 14-8-13.
// Copyright (c) 2014 dantezhu. All rights reserved.
//

#include <string.h>
#include <fcntl.h>
#include <sstream>

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)

#include <winsock2.h>
// for addrinfo
#include <ws2tcpip.h>
#pragma comment(lib,"pthreadVSE2.lib")
#define SOCKET_OPT_LEN_TYPE int
// 不能用 char*，否则编译不过
#define SOCKET_OPT_VAL_PTR_TYPE char

#else

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
// for addrinfo
#include <netdb.h>
#include <poll.h>
#define SOCKET_OPT_LEN_TYPE socklen_t
#define SOCKET_OPT_VAL_PTR_TYPE void

#endif


#include "TcpClient.h"

namespace netkit {

TcpClient::TcpClient(const std::string &host, int port, double timeout) {
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

int TcpClient::connect() {

    netkit::SocketType sockFd;

    int connectResult = _customConnect(m_host, m_port, m_timeout, sockFd);
    if (connectResult != 0) {
        return connectResult;
    }

    if (m_timeout > 0) {
        struct timeval tvTimeout={
                (int)m_timeout,
                (int)((m_timeout - (int)m_timeout) * 1000000)
        };

        setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tvTimeout, sizeof(tvTimeout));
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

void TcpClient::shutdown(int how) {
    if (!m_stream) {
        return;
    }

    return m_stream->shutdown(how);
}

void TcpClient::close() {
    if (!m_stream) {
        return;
    }

    return m_stream->close();
}

bool TcpClient::isClosed() {
    if (!m_stream) {
        return true;
    }

    return m_stream->isClosed();
}

void TcpClient::_setBlockSocket(netkit::SocketType sockFd, bool block) {
#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
    u_long mode = block ? 0 : 1;

    ioctlsocket(sockFd, FIONBIO, &mode);
#else

    int flags = fcntl(sockFd, F_GETFL, 0);
    if (block) {
        // 设置为阻塞
        fcntl(sockFd, F_SETFL, flags & ~O_NONBLOCK);
    }
    else {
        // 设置为非阻塞
        fcntl(sockFd, F_SETFL, flags | O_NONBLOCK);
    }
#endif
}

int TcpClient::_createUnitySocket(std::string host, int port, netkit::SocketType &resultSock, std::string &address) {
    // 参考： https://tools.ietf.org/html/rfc4038 6.3.2
    struct addrinfo hints, *res, *aip;
    netkit::SocketType sockFd;
    std::stringstream ssPort;
    ssPort << port;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), ssPort.str().c_str(), &hints, &res) != 0) {
        // handle getaddrinfo error
        return -1;
    }

    for (aip=res; aip; aip=aip->ai_next) {

        sockFd = socket(aip->ai_family,
                        aip->ai_socktype,
                        aip->ai_protocol);

        if (sockFd < 0) {
            switch errno {
            case EAFNOSUPPORT:
            case EPROTONOSUPPORT:
                // e.g., skip the errors until
                // the last address family,
                // see section 4.4.
                if (aip->ai_next) {
                    continue;
                }
                else {
                    // handle unknown protocol errors
                    break;
                }

            default:
                // handle other socket errors
                ;
            }

        }
        else {
            // 找到socket了
            break;
        }
    }

    if (sockFd > 0) {
        resultSock = sockFd;

        address.resize(aip->ai_addrlen);
        memcpy((void*)address.c_str(), aip->ai_addr, aip->ai_addrlen);
    }

    // 释放
    freeaddrinfo(res);

    if (sockFd > 0) {
        return 0;
    }
    else {
        return -2;
    }
}

int TcpClient::_createV4Socket(std::string host, int port, netkit::SocketType &resultSock, std::string &address) {
    struct sockaddr_in serverAddress;

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port   = htons((unsigned short)port);

    // 验证IP是否合法
    // >0 代表成功
    // 如果函数出错将返回一个负值，并将errno设置为EAFNOSUPPORT，如果参数af指定的地址族和src格式不对，函数将返回0。
    if (inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) <= 0) {
        return -1;
    }

    netkit::SocketType sockFd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockFd < 0) {
        return -2;
    }

    resultSock = sockFd;

    address.resize(sizeof(serverAddress));
    memcpy((void*)address.c_str(), &serverAddress, sizeof(serverAddress));

    return 0;
}

int TcpClient::_createV6Socket(std::string host, int port, netkit::SocketType &resultSock, std::string &address) {
    
    struct sockaddr_in6 serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port   = htons((unsigned short)port);

    if (inet_pton(AF_INET6, host.c_str(), &serverAddress.sin6_addr) <= 0) {
        return -1;
    }

    netkit::SocketType sockFd = socket(AF_INET6, SOCK_STREAM, 0);

    if (sockFd < 0) {
        return -2;
    }

    resultSock = sockFd;

    address.resize(sizeof(serverAddress));
    memcpy((void*)address.c_str(), &serverAddress, sizeof(serverAddress));

    return 0;
}


int TcpClient::_customConnect(std::string host, int port, double timeout, netkit::SocketType &resultSock) {
#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
    return _selectConnect(host, port, timeout, resultSock);
#else
    return _pollConnect(host, port, timeout, resultSock);
#endif
}

int TcpClient::_blockConnect(std::string host, int port, netkit::SocketType &resultSock) {
    // 默认就是ERROR
    int connectResult = -1;
    std::string address;
    netkit::SocketType sockFd;

    // 先尝试用v4的ip创建，如果不行，就认为是域名来创建
    if (_createV4Socket(host, port, sockFd, address) != 0) {

        if (_createUnitySocket(host, port, sockFd, address) != 0) {
            // 失败了
            return connectResult;
        }
    }

    if (sockFd > 0) {
        if (::connect(sockFd, (struct sockaddr*)address.c_str(), (SOCKET_OPT_LEN_TYPE)address.size()) == 0) {
            connectResult = 0;
        }
        else {
            // 失败了
            CLOSE_SOCKET(sockFd);
        }
    }

    // 成功了之后才赋值
    if (connectResult == 0) {
        resultSock = sockFd;
    }

    return connectResult;
}

int TcpClient::_selectConnect(std::string host, int port, double timeout, netkit::SocketType &resultSock) {
    // 默认就是ERROR
    int connectResult = -1;
    std::string address;
    netkit::SocketType sockFd;

    // 先尝试用v4的ip创建，如果不行，就认为是域名来创建
    if (_createV4Socket(host, port, sockFd, address) != 0) {

        if (_createUnitySocket(host, port, sockFd, address) != 0) {
            // 失败了
            return connectResult;
        }
    }

    if (sockFd > 0) {

        // 设置为非阻塞
        _setBlockSocket(sockFd, false);

        if (::connect(sockFd, (struct sockaddr*)address.c_str(), (SOCKET_OPT_LEN_TYPE)address.size()) == -1) {

            // host不存在和port不存在的最大区别时，client发出syn请求后，对方是否有响应.
            // host不存在，syn没有响应
            // port不存在, syn报错
            // linux 默认connect 超时是75秒
#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
            // windows 下不会设置errno，所以无法根据errno判断
            // host/port不存在 都会connect报错，然后进入select
            // host不存在会等待至select超时. onTimeout
            // port不存在会进入errorFDs判断，所以会很快报错. onError

            // 注意: 打印errno的时候，要存起来之后再打印，否则可能两次打印会不一致
#else
            // 其他平台需要判断errno
            // host不存在或者连接进行中会进入到 EINPROGRESS. onTimeout
            // port不存在errno会是别的错误，直接报错. onError
            if (errno == EINPROGRESS)
#endif

            {
                // 进行中，准备用select判断超时
                struct timeval tvTimeout ={
                    (int)timeout,
                    (int)((timeout - (int)timeout) * 1000000)
                };

                // 可写，和报错状态都检测
                fd_set writeFDs, errorFDs;
                FD_ZERO(&writeFDs);
                FD_SET(sockFd, &writeFDs);

                FD_ZERO(&errorFDs);
                FD_SET(sockFd, &errorFDs);

                // windows 下，第一个参数无用，select没有fd数量和fd大小的限制
                // linux下，fd的个数和最大值都不能超过1024
                int ret = select(sockFd + 1, NULL, &writeFDs, &errorFDs, &tvTimeout);
                if(ret > 0){
                    if (FD_ISSET(sockFd, &errorFDs)) {
                        // 报错了
                    }
                    else if (FD_ISSET(sockFd, &writeFDs)) {
                        // 说明找到了
                        int tmpError = 0;
                        SOCKET_OPT_LEN_TYPE tmpLen = sizeof(tmpError);
                        SOCKET_OPT_VAL_PTR_TYPE* ptrTmpError = (SOCKET_OPT_VAL_PTR_TYPE*) &tmpError;

                        // 下面的一句一定要，主要针对防火墙
                        getsockopt(sockFd, SOL_SOCKET, SO_ERROR, ptrTmpError, &tmpLen);

                        if(tmpError==0) {
                            // 成功
                            connectResult = 0;
                        }
                    }
                }
                else if (ret == 0) {
                    // 超时了没返回
                    connectResult = -100;
                }
            }
        }
        else {
            // 成功
            connectResult = 0;
        }

        // 重新设置为阻塞
        _setBlockSocket(sockFd, true);

        if (connectResult != 0) {
            CLOSE_SOCKET(sockFd);
        }
    }

    // 成功了之后才赋值
    if (connectResult == 0) {
        resultSock = sockFd;
    }

    return connectResult;
}

#if !defined(_WIN32) && !(defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)

int TcpClient::_pollConnect(std::string host, int port, double timeout, netkit::SocketType &resultSock) {
    // windows 下没有poll

    // 默认就是ERROR
    int connectResult = -1;
    std::string address;
    netkit::SocketType sockFd;

    // 先尝试用v4的ip创建，如果不行，就认为是域名来创建
    if (_createV4Socket(host, port, sockFd, address) != 0) {

        if (_createUnitySocket(host, port, sockFd, address) != 0) {
            // 失败了
            return connectResult;
        }
    }

    if (sockFd > 0) {

        // 设置为非阻塞
        _setBlockSocket(sockFd, false);

        if (::connect(sockFd, (struct sockaddr*)address.c_str(), (SOCKET_OPT_LEN_TYPE)address.size()) == -1) {

            if (errno == EINPROGRESS) {

                struct pollfd fdList[1];

                struct pollfd &pollSock = fdList[0];

                memset(&pollSock, 0, sizeof(pollSock));
                pollSock.fd = sockFd;
                // 要可写
                pollSock.events = POLLOUT | POLLERR | POLLHUP;

                int ret = ::poll(fdList, 1, (int)(timeout * 1000));
                if(ret > 0){
                    // 说明找到了
                    if ((pollSock.revents & POLLERR) || (pollSock.revents & POLLHUP)) {
                        // 报错了
                    }
                    else if (pollSock.revents & POLLOUT) {
                        int tmpError = 0;
                        SOCKET_OPT_LEN_TYPE tmpLen = sizeof(tmpError);
                        SOCKET_OPT_VAL_PTR_TYPE* ptrTmpError = (SOCKET_OPT_VAL_PTR_TYPE*) &tmpError;

                        // 下面的一句一定要，主要针对防火墙
                        getsockopt(sockFd, SOL_SOCKET, SO_ERROR, ptrTmpError, &tmpLen);

                        if(tmpError==0) {
                            // 成功
                            connectResult = 0;
                        }

                    }
                }
                else if (ret == 0) {
                    // 超时了没返回
                    connectResult = -100;
                }
            }
        }
        else {
            // 成功
            connectResult = 0;
        }

        // 重新设置为阻塞
        _setBlockSocket(sockFd, true);

        if (connectResult != 0) {
            CLOSE_SOCKET(sockFd);
        }
    }

    // 成功了之后才赋值
    if (connectResult == 0) {
        resultSock = sockFd;
    }

    return connectResult;
}

#endif

}
