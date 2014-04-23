# -*- coding: utf-8 -*-

from netkit import bintp
from netkit.stream import Stream

import logging
import socket

logger = logging.getLogger('netkit')
logger.addHandler(logging.StreamHandler())
logger.setLevel(logging.DEBUG)

address = ('127.0.0.1', 7777)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(address)

stream = Stream(s)

tp = bintp.new()
tp.body = '我爱你'

stream.write(tp.pack())

while True:
    # 阻塞
    def read_over(buf):
        print bintp.from_buf(buf)

    stream.read_with_checker(bintp.from_buf, read_over)
    if stream.closed():
        print 'server closed'
        break

s.close()
