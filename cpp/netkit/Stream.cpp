//
// Created by dantezhu on 14-7-26.
// Copyright (c) 2014 dantezhu. All rights reserved.
//

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)

#include <winsock2.h>

#else

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#endif

#include "Stream.h"

namespace netkit {

void Stream::_init(SocketType sockFd, int initReadBufferSize) {
    m_bufferedLength = 0;
    m_sockFd = sockFd;

    initReadBufferSize = initReadBufferSize > 0 ? initReadBufferSize : INIT_READ_BUFFER_SIZE;
    m_readBuffer.resize(initReadBufferSize);
}

Stream::Stream(SocketType sockFd) {
    _init(sockFd, 0);
}

Stream::Stream(SocketType sockFd, int initReadBufferSize) {
    _init(sockFd, initReadBufferSize);
}

Stream::~Stream() {}

int Stream::read(IBox* box) {
    if (isClosed()) {
        return -1;
    }

    if (!box) {
        return -2;
    }

    // 直接只支持
    while(true) {
        if (m_bufferedLength > 0) {
            // 说明还是可以尝试一下的

            int ret = box->unpack(m_readBuffer.c_str(), m_bufferedLength);
            if (ret > 0) {
                // 说明成功

                m_bufferedLength -= ret;

                memmove((char*)m_readBuffer.c_str(), m_readBuffer.c_str() + ret, m_bufferedLength);

                return 0;
            }
            else if (ret < 0) {
                // 说明数据错乱了
                m_bufferedLength = 0;
            }
        }

        if (m_readBuffer.size() <= m_bufferedLength) {
            // 已经没有多余的空间来储存了
            // 每次以翻倍处理
            m_readBuffer.resize(m_readBuffer.size()*2);
        }

        int len = recv(m_sockFd, (char*)m_readBuffer.c_str() + m_bufferedLength, m_readBuffer.size() - m_bufferedLength, 0);
        // -1：当server关闭的时候会报这个错误
        if (len <= 0) {
            // 说明报错了，或者连接失败了

            if (len == -1 && errno == EAGAIN) {
                // 读取超时
                return RET_RECV_TIMEOUT;
            }
            //printf("len: %d\n, errno: %d, %s", len, errno, strerror(errno), EAGAIN);
            closeStream();
            return -3;
        }

        // 进入下个循环自然会判断
        m_bufferedLength += len;
    }

}

int Stream::write(IBox* box) {
    if (!box) {
        return -1;
    }

    std::string str;

    int packetLen = box->pack(str);
    if (packetLen < 0) {
        return -2;
    }

    return write(str.c_str(), str.size());
}

int Stream::write(const char* buf, int bufLen) {

    int sentLen = 0;

    while (sentLen < bufLen) {
        int len = send(m_sockFd, buf + sentLen, bufLen - sentLen, 0);
        if (len < 0) {
            return -3;
        }

        sentLen += len;
    }

    return 0;
}

void Stream::shutdownStream(int how) {
    shutdown(m_sockFd, how);
}

void Stream::closeStream() {
    if (m_sockFd > 0) {
        CLOSE_SOCKET(m_sockFd);
        m_sockFd = 0;
    }
}

bool Stream::isClosed() {
    return m_sockFd <= 0;
}

}
