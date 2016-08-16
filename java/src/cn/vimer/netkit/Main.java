package cn.vimer.netkit;

import java.io.*;
import java.net.Socket;

public class Main {

    private static void testPack() throws IOException {
        Box box = new Box();

        box.cmd = 1;
        box.version = 2;
        //box.body = "woaini".getBytes();

        byte[] buf = box.pack();

        FileOutputStream fileOutputStream = new FileOutputStream("java_pack_result");
        fileOutputStream.write(buf);
        fileOutputStream.close();
    }

    private static void testUnpack() throws IOException {

        FileInputStream fileInputStream = new FileInputStream("python_pack_result");

        byte[] bytes = new byte[1000];
        // 其实应该判断下是否读完的
        int bytes_len = fileInputStream.read(bytes);

        Box box = new Box();

        int ret = box.unpack(bytes, 0, bytes_len);

        System.out.println("ret: " + ret);

        System.out.println("box: " + box);
    }

    private static void testStream() throws IOException{
        Socket socket = new Socket("127.0.0.1", 8533);
        socket.setSoTimeout(10000);

        Stream stream = new Stream(socket);

        Box box = new Box();
        box.cmd = 1;
        box.version = 123;
        box.body = "woaini".getBytes();

        stream.write(box);

        Box box2 = new Box();
        boolean succ = stream.read(box2);
        System.out.println("succ: " + succ);
        System.out.println("box: " + box2);
    }

    private static void testByteDuiQi() throws IOException{
        // 测试字节对齐

        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream(50);
        DataOutputStream outputStream = new DataOutputStream(byteArrayOutputStream);

        outputStream.writeInt(1);
        outputStream.writeShort(3);
        outputStream.writeInt(1);
        outputStream.writeByte(1);
        outputStream.flush();

        byte[] buf = byteArrayOutputStream.toByteArray();

        for (int i=0; i<buf.length; i++) {
            System.out.print(Integer.toHexString(buf[i]) + ",");
        }

        // 结论是没有做字节对齐
    }

    private static void testLineStream() throws IOException{
        Socket socket = new Socket("127.0.0.1", 7777);

        LineStream stream = new LineStream(socket);

        stream.write("我爱你");
        System.out.println(stream.read());
    }

    private static void testLineBox() throws IOException {
        Socket socket = new Socket("127.0.0.1", 7777);

        Stream stream = new Stream(socket);

        LineBox sendBox = new LineBox();
        sendBox.setBody("我们的爱");

        stream.write(sendBox);

        while (!stream.isClosed()) {
            LineBox recvBox = new LineBox();

            boolean ret = stream.read(recvBox);
            if (!ret) {
                continue;
            }
            System.out.println(ret);
            System.out.println(recvBox.getBody());
        }
    }

    public static void main(String[] args) {

        try{
            testStream();
        }
        catch (Exception e) {
            System.out.println("fail: " + e);
        }
    }
}
