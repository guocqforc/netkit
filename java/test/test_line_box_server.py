#!/usr/bin/env python
# -*- coding: utf-8 -*-

import gevent
from gevent.server import StreamServer

class RequestHandler(object):

    closed = False

    def __init__(self, sock, address):
        self.sock = sock
        self.address = address
        self.f = self.sock.makefile('r')
        self.handle()

    def handle(self):
        while not self.closed:
            # 必须要启动一个新的greenlet，在greenlet里面执行readline
            # 否则会有内存泄漏
            t = gevent.spawn(self.read_message)
            t.join()

    def read_message(self):
        message = self.f.readline()
        if not message:
            self.closed = True
            print 'client closed'
            return
        print "message, len: %s, content: %r" % (len(message), message)
        self.sock.send('ok\n')


server = StreamServer(('127.0.0.1', 7777), RequestHandler)
server.serve_forever()
