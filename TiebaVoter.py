# Baidu 投票客户端（Python 版）
import socket
import re
import time
import sys


# ==============================
# 错误处理
# ==============================
def fatal_error(msg: str):
    print(f"[错误] {msg}")
    print("按任意键退出...")
    input()
    sys.exit(1)


# ==============================
# 响应解析
# ==============================
class ResponseParser:
    def __init__(self, data: bytes):
        # 响应可能包含 gzip 压缩数据，先尝试用 latin-1 解码保留字节
        self.text = data.decode("latin-1", errors="ignore")

    def _extract(self, key: str, delim: str) -> str:
        start = self.text.find(key)
        if start == -1:
            return ""
        start += len(key)
        end = self.text.find(delim, start)
        if end == -1:
            end = len(self.text)
        return self.text[start:end]

    def find_sign_id(self) -> str:
        return self._extract("sign_id=", "&")

    def find_vote_id(self) -> str:
        return self._extract("vote_id=", "&")

    def extract_baidu_wise_uid(self) -> str:
        return self._extract("BAIDU_WISE_UID=", ";")

    def extract_is_new_user(self) -> str:
        return self._extract("IS_NEW_USER=", ";")

    def extract_baidu_id(self) -> str:
        return self._extract("BAIDUID=", ";")

    def extract_tbs(self) -> str:
        return self._extract("tbs=", "&")


# ==============================
# 请求构建
# ==============================
class RequestBuilder:
    def __init__(self):
        self.address = ""
        self.select_item = ""
        self.baidu_wise_uid = ""
        self.is_new_user = ""
        self.baidu_id = ""
        self.sign_id = ""
        self.vote_id = ""
        self.tbs = ""

    def build_get_request(self) -> bytes:
        req = (
            f"GET /f/q---wiaui_1339034079_1644--1-1-0/m?kz={self.address}"
            " HTTP/1.1\r\nHost: wapp.baidu.com\r\nConnection: keep-alive\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
            "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
            "Accept-Encoding: gzip,deflate,sdch\r\n"
            "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"
        )
        return req.encode("latin-1")

    def build_post_request(self) -> bytes:
        req = (
            "POST /f/q---wiaui_1339034079_1644--1-1-0/m HTTP/1.1\r\n"
            "Host: wapp.baidu.com\r\nConnection: keep-alive\r\nContent-Length: 198\r\n"
            "Cache-Control: max-age=0\r\nOrigin: http://wapp.baidu.com\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
            "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
            f"Referer: http://wapp.baidu.com/f/q---wiaui_1339034079_1644--1-1-0/m?kz={self.address}"
            "\r\nAccept-Encoding: gzip,deflate,sdch\r\n"
            "Accept-Language: zh-CN,zh;q=0.8\r\n"
            f"Cookie: BAIDU_WISE_UID=wapp_{self.baidu_wise_uid}"
            f"; IS_NEW_USER={self.is_new_user}"
            f"; BAIDUID={self.baidu_id}"
            ":FG=1\r\n\r\n"
            f"select_items={self.select_item}"
            f"&pinf=&sign_id={self.sign_id}"
            f"&can_post=&vote_id={self.vote_id}"
            f"&product_id=1&tbs={self.tbs}"
            "&tn=bdVotPos&z=0000000000&sub1=%E6%8A%95%E7%A5%A8\r\n"
        )
        return req.encode("latin-1")


# ==============================
# 投票客户端
# ==============================
class BaiduVoteClient:
    HOST = "wapp.baidu.com"
    PORT = 80

    def __init__(self):
        self.sock = None

    def connect(self) -> bool:
        self.close()
        try:
            self.sock = socket.create_connection((self.HOST, self.PORT), timeout=10)
            return True
        except OSError:
            self.sock = None
            return False

    def send_data(self, data: bytes) -> bool:
        if self.sock is None:
            return False
        try:
            self.sock.sendall(data)
            return True
        except OSError:
            return False

    def receive_data(self, buffer_size: int = 15000) -> int:
        if self.sock is None:
            return -1
        try:
            data = self.sock.recv(buffer_size - 1)
            if not data:
                return 0
            self.last_recv = data
            return len(data)
        except OSError:
            return -1

    def close(self):
        if self.sock is not None:
            try:
                self.sock.close()
            except OSError:
                pass
            self.sock = None


# ==============================
# 主程序
# ==============================
def main():
    req_builder = RequestBuilder()
    client = BaiduVoteClient()

    address = input("帖子地址ID: ").strip()
    select_item = input("投票选项(数字): ").strip()

    req_builder.address = address
    req_builder.select_item = select_item

    if not client.connect():
        fatal_error("服务器连接失败")

    # 首次 GET，拿 sign_id / vote_id
    client.send_data(req_builder.build_get_request())
    recv_len = client.receive_data()
    if recv_len == -1:
        fatal_error("首次请求失败")

    parser = ResponseParser(client.last_recv)
    req_builder.sign_id = parser.find_sign_id()
    req_builder.vote_id = parser.find_vote_id()

    # 循环投票
    print("开始循环投票...")
    while True:
        client.send_data(req_builder.build_get_request())
        recv_len = client.receive_data()

        # 连接断开自动重连
        if recv_len == 0 or recv_len == -1:
            print("连接断开，正在重连...")
            client.close()
            if not client.connect():
                print("重连失败，重试中...")
                time.sleep(1)
                continue
            continue

        buf = client.last_recv
        # 跳过无效响应
        if not buf or buf[0:1] != b'H':
            continue

        curr_parser = ResponseParser(buf)
        uid = curr_parser.extract_baidu_wise_uid()
        if not uid:
            continue

        req_builder.baidu_wise_uid = uid
        req_builder.is_new_user = curr_parser.extract_is_new_user()
        req_builder.baidu_id = curr_parser.extract_baidu_id()
        req_builder.tbs = curr_parser.extract_tbs()

        # 构建并发送 POST 投票请求
        client.send_data(req_builder.build_post_request())
        client.receive_data()

        print("投票请求已发送")
        time.sleep(1)


if __name__ == "__main__":
    main()
