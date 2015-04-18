# -*- coding: utf-8 -*-

from netkit.box import Box

box1 = Box()
print box1
print repr(box1.pack())

box2 = Box()
box2.unpack(box1.pack())

print box2
