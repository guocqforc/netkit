package cn.vimer.netkit;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.ByteArrayInputStream;
import java.io.IOException;


// 与python的netkit不同
// java版只认网络字节序，因为java的所有存储都是以网络字节序
// 只支持packet_len

public class Box implements IBox {

    // 魔法数字
    public final static int MAGIC = 2037952207;

    // 默认就用这个值
    public int magic = MAGIC;
    public short version;
    public short flag;
    // 这个只用来在网络上传输，放在这里仅作占位符
    private int _placeholder_packet_len;
    public int cmd;
    public int ret;
    public int sn;

    // 包体
    public byte[] body = null;

    // 如果解析完毕会被设置为true
    private boolean _unpack_done = false;

    // 压包
    public byte[] pack() {

        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream(this.packetLen());
        DataOutputStream outputStream = new DataOutputStream(byteArrayOutputStream);

        try{
            outputStream.writeInt(magic);
            outputStream.writeShort(version);
            outputStream.writeShort(flag);
            outputStream.writeInt(packetLen());
            outputStream.writeInt(cmd);
            outputStream.writeInt(ret);
            outputStream.writeInt(sn);
            if (body != null) {
                outputStream.write(body);
            }

            outputStream.flush();
        }
        catch (Exception e) {
            return null;
        }

        return byteArrayOutputStream.toByteArray();
    }

    // 解包
    public int unpack(byte[] buf) {
        return unpack(buf, 0, buf == null ? 0:buf.length);
    }

    public int unpack(byte[] buf, int offset, int length) {
        return _unpack(buf, offset, length, true);
    }

    //>0: 成功生成obj，返回了使用的长度，即剩余的部分buf要存起来
    //<0: 报错
    //0: 继续收
    public int _unpack(byte[] buf, int offset, int length, boolean save) {

        if (buf == null) {
            return -100;
        }

        length = buf.length > length ? length : buf.length;

        if (length < this.headerLen()) {
            return 0;
        }

        DataInputStream inputStream = new DataInputStream(new ByteArrayInputStream(buf, offset, length));

        int _magic;
        short _version;
        short _flag;
        int _packet_len;
        int _cmd;
        int _ret;
        int _sn;
        int _body_len;
        byte[] _body = null;

        try {
            _magic = inputStream.readInt();
            _version = inputStream.readShort();
            _flag = inputStream.readShort();
            _packet_len = inputStream.readInt();
            _cmd = inputStream.readInt();
            _ret = inputStream.readInt();
            _sn = inputStream.readInt();
        }
        catch (Exception e) {
            return -1;
        }

        if (_magic != MAGIC) {
            // 包错误
            return -2;
        }

        if (length < _packet_len) {
            // 继续收
            return 0;
        }

        _body_len = _packet_len - this.headerLen();

        try{
            if (_body_len > 0) {
                _body = new byte[_body_len];
                int len = inputStream.read(_body);

                // 说明解出来的body长度不对
                if (len != _body_len) {
                    System.out.println(len);
                    return -3;
                }
            }
        }
        catch (Exception e) {
            // 解body异常
            return -4;
        }

        if (!save) {
            return _packet_len;
        }

        this.magic = _magic;
        this.version = _version;
        this.flag = _flag;
        this._placeholder_packet_len = _packet_len;
        this.cmd = _cmd;
        this.ret = _ret;
        this.sn = _sn;
        this.body = _body;

        this._unpack_done = true;

        return _packet_len;
    }

    // 检查
    public int check(byte[] buf) {
        return check(buf, 0, buf == null ? 0:buf.length);
    }

    // 检查
    public int check(byte[] buf, int offset, int length) {
        return _unpack(buf, offset, length, false);
    }

    public int headerLen() {
        return 4 * 5 + 2 * 2;
    }

    public int bodyLen() {
        if (body != null) {
            return body.length;
        }

        return 0;
    }

    public int packetLen() {
        return headerLen() + bodyLen();
    }

    public boolean unpackDone() {
        return _unpack_done;
    }

    public JSONObject getJson() throws IOException, JSONException {
        if (body == null) {
            return null;
        }

        return new JSONObject(new String(body, "UTF-8"));
    }

    public void setJson(JSONObject jsonObject) {
        if (jsonObject == null) {
            body = null;
        }

        body = jsonObject.toString().getBytes();
    }

    public String toString() {
        String out = "";
        out += "magic: " + magic;
        out += ", version: " + version;
        out += ", flag: " + flag;
        out += ", packetLen: " + packetLen();
        out += ", cmd: " + cmd;
        out += ", ret: " + ret;
        out += ", sn: " + sn;
        out += ", bodyLen: " + bodyLen();

        return out;
    }
}
