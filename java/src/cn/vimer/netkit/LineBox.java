package cn.vimer.netkit;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

/**
 * Created by dantezhu on 14-7-3.
 */
public class LineBox implements IBox {

    private static final int LINE_END = 10; // \n

    // 如果解析完毕会被设置为true
    private boolean _unpack_done = false;

    // 包体
    private byte[] body = null;

    public boolean unpackDone() {
        return this._unpack_done;
    }

    public void setBody(String body_) {
        if (body_ == null) {
            this.body = null;
            return;
        }

        String str_line_end = String.valueOf((char)LINE_END);

        if (!body_.endsWith(str_line_end)) {
            body_ += str_line_end;
        }

        this.body = body_.getBytes();
    }

    public String getBody() {
        return new String(this.body);
    }

    public byte[] pack() {
        if (this.body == null) {
            return null;
        }

        return this.body;
    }

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

        int found_len = 0;

        length = buf.length > length ? length : buf.length;

        for (int i = offset; i < length; i++) {
            if (buf[i] == LINE_END)
            {
                found_len = i - offset + 1;
                break;
            }
        }

        if (found_len == 0) {
            // 继续收
            return 0;
        }

        if (!save) {
            return found_len;
        }

        this.body = new byte[found_len];
        // copy进去
        System.arraycopy(buf, offset, this.body, 0, found_len);
        this._unpack_done = true;

        return found_len;
    }

    public int check(byte[] buf) {
        return check(buf, 0, buf == null ? 0:buf.length);
    }
    public int check(byte[] buf, int offset, int length) {
        return _unpack(buf, offset, length, false);
    }

    public JSONObject getJson() throws IOException, JSONException {
        if (getBody() == null) {
            return null;
        }

        return new JSONObject(getBody());
    }

    public void setJson(JSONObject jsonObject) {
        if (jsonObject == null) {
            setBody(null);
        }

        setBody(jsonObject.toString());
    }

    public String toString() {
        // 为了防止不要crash
        return String.valueOf(this.body);
    }
}
