# -*- coding: utf-8 -*-
#!/usr/bin/env python
# -*- coding: utf-8 -*-


from gevent import monkey;monkey.patch_all()

from netkit.stream import Stream
import gevent
from gevent.server import StreamServer
from reimp import logger, Box


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
                    req_box.body = 'ok'

                    buf = req_box.pack()
                    self.stream.write(buf[:2])
                    self.stream.write(buf[2:])

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
