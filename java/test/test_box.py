#!/usr/bin/env python
# -*- coding: utf-8 -*-

from netkit.box import Box

def test_pack():
    box = Box()

    box.cmd = 1
    box.version = 1000
    box.ret = 2

    buf = box.pack()

    with open('python_pack_result', 'wb') as f:
        f.write(buf)

def test_unpack():
    box = Box()

    with open("java_pack_result", "rb") as f:
        buf = f.read()

        print box.unpack(buf)
        print box


def main():
    test_unpack()


if __name__ == '__main__':
    main()
