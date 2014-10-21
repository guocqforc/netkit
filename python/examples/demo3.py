# -*- coding: utf-8 -*-

from netkit.box import Box


box1 = Box()
box1.body = '我爱你'
print box1
print repr(box1.pack())
print box1.body

buf = box1.pack()

box2 = Box()
for i in range(0, len(buf)+1):
    tmp_buf = buf[0:i]

    if box2.unpack(tmp_buf) > 0:
        print box2
        break
