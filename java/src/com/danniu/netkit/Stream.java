package com.danniu.netkit;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.Socket;

public class Stream {

    private final static int READ_CHUNK_SIZE = 4 * 1024;

    private Socket socket = null;

    private byte[] readChunkBuffer = null;

    private ByteArrayOutputStream readBufferOutputStream;

    public Stream(Socket socket) {
        this(socket, 0);
    }

    public Stream(Socket socket, int readChunkSize) {
        readBufferOutputStream = new ByteArrayOutputStream();

        readChunkSize = readChunkSize > 0 ? readChunkSize : READ_CHUNK_SIZE;

        // readChunkSize 一次性读取的大小
        readChunkBuffer = new byte[readChunkSize];
        this.socket = socket;
    }

    public boolean read(IBox box) throws IOException {
        if (this.isClosed()) {
            return false;
        }

        // 直接只支持
        while(true) {
            byte[] tmpBuffer = readBufferOutputStream.toByteArray();
            if (tmpBuffer.length > 0) {
                // 说明还是可以尝试一下的

                int ret = box.unpack(tmpBuffer, 0, tmpBuffer.length);
                if (ret > 0) {
                    // 说明成功
                    readBufferOutputStream.reset();
                    readBufferOutputStream.write(tmpBuffer, ret, tmpBuffer.length-ret);

                    return true;
                }
                else if (ret < 0) {
                    // 说明数据错乱了
                    readBufferOutputStream.reset();
                }
            }

            int len = this.socket.getInputStream().read(readChunkBuffer, 0, readChunkBuffer.length);
            // 如果读取超时会抛出异常：java.net.SocketTimeoutException，不会进入下面的逻辑
            // -1：当server关闭的时候会报这个错误
            if (len <= 0) {
                // 说明报错了，或者连接失败了
                try {
                    this.close();
                }
                catch (Exception e) {
                }
                return false;
            }

            // 进入下个循环自然会判断
            readBufferOutputStream.write(readChunkBuffer, 0, len);
        }
    }

    public void write(IBox box) throws IOException{
        if (this.isClosed()) {
            throw new IOException("closed");
        }

        this.socket.getOutputStream().write(box.pack());
        this.socket.getOutputStream().flush();
    }

    public void close() throws IOException {
        if (this.socket != null) {
            this.socket.close();
            this.socket = null;
        }
    }

    public boolean isClosed() {
        // 之所以删掉socket.isClosed() 判断是因为不准
        return this.socket == null;
    }

    public void shutdown(int how) {
        if (this.socket != null) {
            if (how == 0 || how == 2) {
                try {
                    this.socket.shutdownInput();
                }
                catch (IOException e) {
                }
            }
            if (how == 1 || how == 2) {
                try {
                    this.socket.shutdownOutput();
                }
                catch (IOException e) {
                }
            }
        }
    }
}
