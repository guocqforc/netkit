package cn.vimer.netkit;

/**
 * Created by dantezhu on 14-7-3.
 */
public interface IBox {

    public byte[] pack();

    public int unpack(byte[] buf);
    public int unpack(byte[] buf, int offset, int length);


    public int check(byte[] buf);
    public int check(byte[] buf, int offset, int length);
}
