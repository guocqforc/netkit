# -*- coding: utf-8 -*-

from netkit.box import Box


box1 = Box()
box1.body = '我爱你'
print box1
print repr(box1.pack())
print box1.body

box2 = Box()
box2.unpack(box1.pack())
print box2
print box2.body
print box2.magic
