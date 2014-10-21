package com.danniu.netkit;

import java.io.*;
import java.net.Socket;

public class LineStream {

    private Socket socket = null;
    private BufferedReader bufferedReader = null;
    private BufferedWriter bufferedWriter = null;

    public LineStream(Socket socket) throws IOException {
        this.socket = socket;

        bufferedReader = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));
        bufferedWriter = new BufferedWriter(new OutputStreamWriter(this.socket.getOutputStream()));
    }

    public String read() throws IOException {
        if (this.isClosed()) {
            return null;
        }

        String line = bufferedReader.readLine();
        if (line == null || line.isEmpty()) {
            try {
                this.close();
            }
            catch (Exception e) {
            }
        }

        return line;
    }


    public void write(String data) throws IOException{
        if (this.isClosed()) {
            throw new IOException("closed");
        }

        if (!data.endsWith("\n"))
        {
            data += "\n";
        }

        bufferedWriter.write(data);
        // 强制刷新
        bufferedWriter.flush();
    }

    public void close() throws IOException {
        // 关闭并为null
        if (this.socket != null) {
            this.socket.close();
            this.socket = null;
        }

        if (this.bufferedReader != null) {
            this.bufferedReader.close();
            this.bufferedReader = null;
        }

        if (this.bufferedWriter != null) {
            this.bufferedWriter.close();
            this.bufferedWriter = null;
        }
    }

    public boolean isClosed() {
        // 之所以删掉socket.isClosed() 判断是因为不准
        return this.socket == null;
    }

    public void shutdownInput() throws IOException {
        if (this.socket != null) {
            this.socket.shutdownInput();
        }
    }

    public void shutdownOutput() throws IOException {
        if (this.socket != null) {
            this.socket.shutdownOutput();
        }
    }
}
