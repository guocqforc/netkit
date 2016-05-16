netkit
======

useful kit for network programming


### 注意

1. read报错或者返回空时，stream内部会自动调用close函数，所以closed()会为True；而write失败，会返回False，但是并不会自动调用close函数。
