# -*- coding: utf-8 -*-
#!/usr/bin/env python
# -*- coding: utf-8 -*-


from gevent import monkey;monkey.patch_all()

from netkit.box import Box
from netkit.stream import Stream
import gevent
from gevent.server import StreamServer
import logging

logger = logging.getLogger('netkit')
logger.addHandler(logging.StreamHandler())
logger.addHandler(logging.FileHandler('s.log'))
logger.setLevel(logging.DEBUG)


class Connection(object):

    def __init__(self, sock, address):
        self.stream = Stream(sock)
        self.process()

    def process(self):
        while not self.stream.closed():
            # 必须要启动一个新的greenlet，在greenlet里面执行readline
            # 否则会有内存泄漏
            def spawn_read():
                message = self.stream.read_with_checker(Box().check)
                #print "message, len: %s, content: %r" % (len(message), message)

                if message:
                    req_box = Box()
                    req_box.unpack(message)
                    print req_box
                    rsp_box = Box()
                    rsp_box.cmd = req_box.cmd
                    rsp_box.ret = 1000
                    rsp_box.version = 333
                    rsp_box.body = 'ok'

                    buf = rsp_box.pack()
                    self.stream.write(buf[:10])
                    self.stream.write(buf[10:])

                if self.stream.closed():
                    print 'client closed'
                    # 说明客户端断掉链接了
                    return

            t = gevent.spawn(spawn_read)
            t.join()


        # 即使不调用close，handle结束也会自动关闭
        #sock.close()


server = StreamServer(('127.0.0.1', 7777), Connection)
server.serve_forever()
