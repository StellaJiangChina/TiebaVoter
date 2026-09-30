# Baidu 投票客户端（requests 版）
import time
import sys
import requests


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
    def __init__(self, text: str):
        self.text = text

    def _extract(self, key: str, delim: str) -> str:
        start = self.text.find(key)
        if start == -1:
            return ""
        start += len(key)
        end = self.text.find(delim, start)
        if end == -1:
            end = len(self.text)
        return self.text[start:end]

    def find_sign_id(self) -> str:            return self._extract("sign_id=", "&")
    def find_vote_id(self) -> str:            return self._extract("vote_id=", "&")
    def extract_baidu_wise_uid(self) -> str:  return self._extract("BAIDU_WISE_UID=", ";")
    def extract_is_new_user(self) -> str:     return self._extract("IS_NEW_USER=", ";")
    def extract_baidu_id(self) -> str:        return self._extract("BAIDUID=", ";")
    def extract_tbs(self) -> str:             return self._extract("tbs=", "&")


# ==============================
# 投票客户端
# ==============================
class BaiduVoteClient:
    BASE_URL = "http://wapp.baidu.com/f/q---wiaui_1339034079_1644--1-1-0/m"

    def __init__(self):
        self.session = requests.Session()
        self.session.headers.update({
            "User-Agent": "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 "
                          "(KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21",
            "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
            "Accept-Language": "zh-CN,zh;q=0.8",
            "Accept-Charset": "GBK,utf-8;q=0.7,*;q=0.3",
            # 注意：不要加 Accept-Encoding: gzip，否则 requests 会自动解压，
            # 但服务器返回的压缩数据可能影响字段查找；保持默认让它自动处理即可。
        })

    def get(self, address: str) -> str:
        """首次 GET / 循环刷新"""
        try:
            resp = self.session.get(self.BASE_URL, params={"kz": address}, timeout=10)
            return resp.text
        except requests.RequestException:
            return ""

    def post_vote(self, address: str, select_item: str,
                  sign_id: str, vote_id: str, tbs: str) -> str:
        """发送 POST 投票"""
        data = {
            "select_items": select_item,
            "pinf": "",
            "sign_id": sign_id,
            "can_post": "",
            "vote_id": vote_id,
            "product_id": "1",
            "tbs": tbs,
            "tn": "bdVotPos",
            "z": "0000000000",
            "sub1": "投票",
        }
        headers = {
            "Referer": f"{self.BASE_URL}?kz={address}",
            "Origin": "http://wapp.baidu.com",
            "Content-Type": "application/x-www-form-urlencoded",
        }
        try:
            resp = self.session.post(self.BASE_URL, data=data,
                                     headers=headers, timeout=10)
            return resp.text
        except requests.RequestException:
            return ""


# ==============================
# 主程序
# ==============================
def main():
    client = BaiduVoteClient()

    address = input("帖子地址ID: ").strip()
    select_item = input("投票选项(数字): ").strip()

    # 首次 GET，拿 sign_id / vote_id
    text = client.get(address)
    if not text:
        fatal_error("首次请求失败")

    parser = ResponseParser(text)
    sign_id = parser.find_sign_id()
    vote_id = parser.find_vote_id()

    # 循环投票
    print("开始循环投票...")
    while True:
        text = client.get(address)
        if not text or not text.startswith("HTTP") and "<html" not in text.lower():
            # 简单有效性检查
            time.sleep(1)
            continue

        curr = ResponseParser(text)
        uid = curr.extract_baidu_wise_uid()
        if not uid:
            time.sleep(1)
            continue

        tbs = curr.extract_tbs()
        client.post_vote(address, select_item, sign_id, vote_id, tbs)

        print("投票请求已发送")
        time.sleep(1)


if __name__ == "__main__":
    main()
