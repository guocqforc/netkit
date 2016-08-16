//
//  main.cpp
//  netkit
//
//  Created by dantezhu on 14-7-26.
//  Copyright (c) 2014 dantezhu. All rights reserved.
//
#if 1

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)

#include <winsock2.h>

#else
#include <arpa/inet.h>
#endif

#include "Box.h"
#include "LineBox.h"
#include "Stream.h"
#include "TcpClient.h"

void test_box() {
    netkit::Box box1;

    box1.version = 10;
    box1.cmd = 1;
    box1.sn = 1;
    box1.setBody("woaini", 5);

    printf("box1: %s\n", box1.toString().c_str());

    std::string strBuf;
    int ret = box1.pack(strBuf);

    printf("box1 pack ret: %d\n", ret);

    netkit::Box box2;

    ret = box2.unpack(strBuf.c_str(), strBuf.size());

    printf("box2 unpack ret: %d, %s\n", ret, box2.toString().c_str());

}

void test_line_box() {
    netkit::LineBox box1;

    box1.setBody("come", 4);

    printf("box1: %s\n", box1.toString().c_str());

    std::string strBuf;
    int ret = box1.pack(strBuf);

    printf("box1 pack ret: %d\n", ret);

    netkit::LineBox box2;

    ret = box2.unpack(strBuf.c_str(), strBuf.size());

    printf("box2 unpack ret: %d, %s\n", ret, box2.toString().c_str());
}

int get_connected_socket(std::string host, int port, netkit::SocketType& sockFd) {

    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(port);
    inet_aton(host.c_str(), (struct in_addr *) &serv_addr.sin_addr.s_addr);

/*
#if !(defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32))
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0)
    {
        printf("inet_pton set ip error:%s\n",host.c_str());
        return -1;
    }
#endif
*/

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if ( sockFd < 0 )
    {
        printf("create socket error:%d\n", errno);
        return -2;
    }

    int ret = connect(sockFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret != 0) {
        printf("connect fail, ret:%d\n", ret);
        return -3;
    }

    return 0;
}


int test_line_box_stream() {

    std::string host = "127.0.0.1";
    int port = 7777;

    netkit::SocketType sockFd;
    int ret = get_connected_socket(host, port, sockFd);
    if (ret) {
        return -1;
    }

    netkit::Stream stream(sockFd);

    netkit::LineBox box1;
    box1.setBody("woaini", 6);
    ret = stream.write(&box1);
    if (ret) {
        printf("write fail, ret:%d\n", ret);
        return -4;
    }

    netkit::LineBox box2;
    ret = stream.read(&box2);
    if (ret) {
        printf("read fail, ret:%d\n", ret);
        return -5;
    }

    printf("read box: %s\n", box2.toString().c_str());

    return 0;
}

int test_box_stream() {


    // 用来测试连接超时
    // std::string host = "4.248.234.2";
    std::string host = "127.0.0.1";
    
    int port = 7777;

    netkit::TcpClient client(host, port, 5);

    int ret;
    ret = client.connect();
    if (ret != 0) {
        printf("connect fail, ret:%d\n", ret);
        return -1;
    }

    netkit::Box box1;
    box1.version = 10;
    box1.flag = 0x9f;
    box1.cmd = 1;
    box1.setBody("woaini", 6);
    ret = client.write(&box1);
    if (ret) {
        printf("write fail, ret:%d\n", ret);
        return -4;
    }

    netkit::Box box2;
    ret = client.read(&box2);
    if (ret) {
        printf("read fail, ret:%d\n", ret);
        return -5;
    }

    printf("read box: %s\n", box2.toString().c_str());

    return 0;
}

int main(int argc, const char * argv[])
{

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
    netkit::Stream::startupSocket();
#endif

    //test_box();
    //test_line_box();
    //test_line_box_stream();
    test_box_stream();

#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
    netkit::Stream::cleanupSocket();
#endif

    return 0;
}

#endif
