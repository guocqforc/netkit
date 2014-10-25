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
    TcpClient(const std::string &host, short port, double timeout);
    virtual ~TcpClient();

    int connectServer();

    // 0 为成功
    int read(IBox* box);

    // 0 为成功
    int write(IBox* box);
    int write(const char* buf, int bufLen);

    void shutdownStream(int how);

    void closeStream();

    bool isClosed();


private:
    std::string m_host;
    short m_port;
    double m_timeout;

    Stream *m_stream;
};

}

#endif //__TcpClient_H_
