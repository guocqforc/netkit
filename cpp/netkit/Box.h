//
// Created by dantezhu on 14-7-26.
// Copyright (c) 2014 dantezhu. All rights reserved.
//


#ifndef BOX_H_20140814144610
#define BOX_H_20140814144610

#include "IBox.h"

namespace netkit {

class Box: public IBox {
public:
    // 魔法数字
    static const int MAGIC = 2037952207;

    // 默认就用这个值
    int magic;
    short version;
    short flag;
    // 这个只用来在网络上传输，放在这里是怕忘记了
    int _transfer_packet_len;
    int cmd;
    int ret;
    int sn;

private:
    // 包体
    std::string _body;

    // 如果解析完毕会被设置为true
    bool _unpack_done;

public:
    Box();
    virtual ~Box();

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

#endif //__Box_H_
