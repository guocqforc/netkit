# -*- coding: utf-8 -*-

from netkit.stream import Stream
from reimp import logger, Box

import socket

address = ('127.0.0.1', 7777)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(address)

stream = Stream(s)

box = Box()
box.body = '我爱你'

stream.write(box.pack())

while 1:
    # 阻塞
    buf = stream.read_with_checker(Box().unpack)

    if buf:
        box = Box()
        box.unpack(buf)
        print box

    if stream.closed():
        print 'server closed'
        break

s.close()
