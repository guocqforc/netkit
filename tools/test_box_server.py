#!/usr/bin/env python
# -*- coding: utf-8 -*-

from haven import GHaven
from netkit.box import Box
import time

import logging

logger = logging.getLogger('haven')
logger.addHandler(logging.StreamHandler())
logger.setLevel(logging.DEBUG)

app = GHaven(Box)

@app.route(1)
def index(request):
    print request.box

    box = Box()
    # box = request.box
    box.ret = 100
    box.body = "wokao" * 1000
    print box.header_len
    print box.packet_len

    print box

    # request.write(box.pack())
    # return

    box.version = 219
    box.flag = 199
    buf = box.pack()
    print '1'
    request.write(buf[:10])
    time.sleep(2)
    print '2'
    request.write(buf[10:])


app.run('127.0.0.1', 7777)
