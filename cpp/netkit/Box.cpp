//
// Created by dantezhu on 14-7-26.
// Copyright (c) 2014 dantezhu. All rights reserved.
//

#include <sstream>
#include <string.h>
#if defined(_WIN32) || (defined(CC_TARGET_PLATFORM) && CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "Box.h"

#define box_mcp(dst, value) \
    memcpy(dst, &(value), sizeof(value)); \
    dst += sizeof(value);

namespace netkit {

Box::Box() {
    this->magic = MAGIC;
    this->version = 0;
    this->flag = 0;
    this->_placeholder_packet_len = 0;
    this->cmd = 0;
    this->ret = 0;
    this->sn = 0;
    this->_unpack_done = false;
}

Box::~Box() {}

int Box::pack(std::string& str) {
    str.resize(this->packetLen());

    char* buf = (char*)str.c_str();

    int i_temp;
    short s_temp;

    i_temp = htonl(this->magic);
    box_mcp(buf, i_temp);

    s_temp = htons(this->version);
    box_mcp(buf, s_temp);

    s_temp = htons(this->flag);
    box_mcp(buf, s_temp);

    i_temp = htonl(this->packetLen());
    box_mcp(buf, i_temp);

    i_temp = htonl(this->cmd);
    box_mcp(buf, i_temp);

    i_temp = htonl(this->ret);
    box_mcp(buf, i_temp);

    i_temp = htonl(this->sn);
    box_mcp(buf, i_temp);

    memcpy(buf, this->getBody(), this->bodyLen());
    buf += this->bodyLen();

    return this->packetLen();
}

int Box::unpack(const char* buf, int length) {
    return this->_unpack(buf, length, true);
}


int Box::check(const char* buf, int length) {
    return this->_unpack(buf, length, false);
}

int Box::_unpack(const char* buf, int length, bool save) {
    if (length < this->headerLen()) {
        // 连包头都不够
        return 0;
    }

    int magic_ = ntohl(*((int*)buf));
    buf += sizeof(int);

    if (magic_ != MAGIC) {
        // 说明magic不对
        return -1;
    }

    short version_ = ntohs(*((short*)buf));
    buf += sizeof(short);

    short flag_ = ntohs(*((short*)buf));
    buf += sizeof(short);

    int packet_len_ = ntohl(*((int*)buf));
    buf += sizeof(int);

    int cmd_ = ntohl(*((int*)buf));
    buf += sizeof(int);

    int ret_ = ntohl(*((int*)buf));
    buf += sizeof(int);

    int sn_ = ntohl(*((int*)buf));
    buf += sizeof(int);

    if (packet_len_ > length) {
        // 还没收完，继续
        return 0;
    }

    if (!save) {
        return packet_len_;
    }

    this->magic = magic_;
    this->version = version_;
    this->flag = flag_;
    this->_placeholder_packet_len = packet_len_;
    this->cmd = cmd_;
    this->ret = ret_;
    this->sn = sn_;

    this->setBody(buf, packet_len_ - this->headerLen());

    this->_unpack_done = true;

    return packet_len_;
}

const char* Box::getBody() {
    return this->_body.c_str();
}

const std::string& Box::getStringBody() {
    return this->_body;
}

void Box::setBody(const char* p_body, int body_len) {
    if (!p_body) {
        this->_body.resize(0);
        return;
    }

    this->_body.assign(p_body, body_len);
}

void Box::setBody(const std::string& str) {
    setBody(str.c_str(), str.size());
}

int Box::headerLen() {
    return sizeof(this->magic) + sizeof(this->version) + sizeof(this->flag) + sizeof(this->_placeholder_packet_len) + \
        sizeof(this->cmd) + sizeof(this->ret) + sizeof(this->sn);
}

int Box::bodyLen() {
    return this->_body.size();
}

int Box::packetLen() {
    return this->headerLen() + this->bodyLen();
}

bool Box::unpackDone() {
    return this->_unpack_done;
}

std::string Box::toString() {
    std::stringstream ss;
    ss << "magic: " << this->magic;
    ss << ", version: " << this->version;
    ss << ", flag: " << this->flag;
    ss << ", cmd: " << this->cmd;
    ss << ", ret: " << this->ret;
    ss << ", sn: " << this->sn;
    ss << ", bodyLen: " << this->bodyLen();

    return ss.str();
}

}
