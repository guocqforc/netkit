# -*- coding: utf-8 -*-

from netkit.line_box import LineBox
from netkit.stream import Stream

import time
import logging
import socket

logger = logging.getLogger('netkit')
logger.addHandler(logging.StreamHandler())
logger.setLevel(logging.DEBUG)

address = ('127.0.0.1', 7777)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(address)

stream = Stream(s)

box = LineBox()
box.body = u'你好吗'

stream.write(box.pack())

while True:
    # 阻塞
    buf = stream.read_with_checker(LineBox().check)

    if buf:
        box = LineBox()
        box.unpack(buf)
        print box

    if stream.closed():
        print 'server closed'
        break

s.close()
