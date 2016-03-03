# -*- coding: utf-8 -*-

from netkit.contrib.tcp_client import TcpClient
from reimp import logger, Box


tcp_client = TcpClient(Box, address=('127.0.0.1', 7777))
# tcp_client = TcpClient(Box, '127.0.0.1', 7777)

tcp_client.connect()

box = Box()
box.body = '我爱你'

tcp_client.write(box)

while 1:
    # 阻塞
    box = tcp_client.read()

    print box

    if not box:
        print 'server closed'
        break

tcp_client.close()
