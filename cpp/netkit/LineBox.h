//
// Created by dantezhu on 14-7-26.
// Copyright (c) 2014 dantezhu. All rights reserved.
//

#ifndef LINEBOX_H_20140814144636
#define LINEBOX_H_20140814144636


#include "IBox.h"
#include <iostream>

namespace netkit {


class LineBox: public IBox {

public:
    static const int LINE_END = 10;

private:
    // 包体
    std::string _body;

    // 如果解析完毕会被设置为true
    bool _unpack_done;

public:
    LineBox();
    virtual ~LineBox();

    virtual int pack(std::string& str);

    virtual int unpack(const char* buf, int length);

    virtual int check(const char* buf, int length);

    const char* getBody();
    const std::string& getStringBody();
    void setBody(const char* p_body, int body_len);
    void setBody(const std::string& str);

    int headerLen();
    int bodyLen();
    int packetLen();

    bool unpackDone();
    std::string toString();

private:
    //>0: 成功生成obj，返回了使用的长度，即剩余的部分buf要存起来
    //<0: 报错
    //0: 继续收
    int _unpack(const char* buf, int length, bool save);
};

}

#endif //__LineBox_H_
