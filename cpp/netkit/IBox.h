//
// Created by dantezhu on 14-7-26.
// Copyright (c) 2014 dantezhu. All rights reserved.
//


#ifndef IBOX_H_20140814144626
#define IBOX_H_20140814144626


#include <iostream>

namespace netkit {

class IBox {
public:
    virtual ~IBox(){}
    // >=0: buf的长度
    // <0:  报错
    virtual int pack(std::string& str)=0;

    virtual int unpack(const char* buf, int length)=0;

    virtual int check(const char* buf, int length)=0;

    virtual std::string toString()=0;
};

}

#endif //__IBox_H_
