// MFC CSocket 版本（精简版）
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _AFXDLL
#include <afxwin.h>
#include <afxsock.h>
#include <iostream>
#include <string>
#include <string_view>

// ==============================
// 错误处理
// ==============================
class ErrorHandler {
public:
    static void fatalError(const std::string& msg) {
        std::cout << "[错误] " << msg << "\n按任意键退出..." << std::endl;
        std::cin.get();
        exit(EXIT_FAILURE);
    }
};

// ==============================
// 请求构建
// ==============================
class RequestBuilder {
    std::string m_address, m_selectItem, m_baiduWiseUid;
    std::string m_isNewUser, m_baiduId, m_signId, m_voteId, m_tbs;

public:
    void setAddress(const std::string& v) { m_address = v; }
    void setSelectItem(const std::string& v) { m_selectItem = v; }
    void setBaiduWiseUid(const std::string& v) { m_baiduWiseUid = v; }
    void setIsNewUser(const std::string& v) { m_isNewUser = v; }
    void setBaiduId(const std::string& v) { m_baiduId = v; }
    void setSignId(const std::string& v) { m_signId = v; }
    void setVoteId(const std::string& v) { m_voteId = v; }
    void setTbs(const std::string& v) { m_tbs = v; }

    std::string buildGetRequest() const {
        return "GET /f/q---wiaui_1339034079_1644--1-1-0/m?kz=" + m_address +
            " HTTP/1.1\r\nHost: wapp.baidu.com\r\nConnection: keep-alive\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
            "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
            "Accept-Encoding: gzip,deflate,sdch\r\n"
            "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n";
    }

    std::string buildPostRequest() const {
        return "POST /f/q---wiaui_1339034079_1644--1-1-0/m HTTP/1.1\r\n"
            "Host: wapp.baidu.com\r\nConnection: keep-alive\r\nContent-Length: 198\r\n"
            "Cache-Control: max-age=0\r\nOrigin: http://wapp.baidu.com\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
            "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
            "Referer: http://wapp.baidu.com/f/q---wiaui_1339034079_1644--1-1-0/m?kz=" + m_address +
            "\r\nAccept-Encoding: gzip,deflate,sdch\r\n"
            "Accept-Language: zh-CN,zh;q=0.8\r\nCookie: BAIDU_WISE_UID=wapp_" + m_baiduWiseUid +
            "; IS_NEW_USER=" + m_isNewUser +
            "; BAIDUID=" + m_baiduId +
            ":FG=1\r\n\r\nselect_items=" + m_selectItem +
            "&pinf=&sign_id=" + m_signId +
            "&can_post=&vote_id=" + m_voteId +
            "&product_id=1&tbs=" + m_tbs +
            "&tn=bdVotPos&z=0000000000&sub1=%E6%8A%95%E7%A5%A8\r\n";
    }
};

// ==============================
// 响应解析
// ==============================
class ResponseParser {
    std::string m_data;

    std::string extract(std::string_view key, char delim) const {
        size_t start = m_data.find(key);
        if (start == std::string::npos) return {};
        start += key.length();
        size_t end = m_data.find(delim, start);
        if (end == std::string::npos) end = m_data.length();
        return m_data.substr(start, end - start);
    }

public:
    explicit ResponseParser(const char* buf) : m_data(buf) {}

    std::string findSignId() { return extract("sign_id=", '&'); }
    std::string findVoteId() { return extract("vote_id=", '&'); }
    std::string extractBaiduWiseUid() { return extract("BAIDU_WISE_UID=", ';'); }
    std::string extractIsNewUser() { return extract("IS_NEW_USER=", ';'); }
    std::string extractBaiduId() { return extract("BAIDUID=", ';'); }
    std::string extractTbs() { return extract("tbs=", '&'); }
};

// ==============================
// 投票客户端
// ==============================
class BaiduVoteClient {
    CSocket m_socket;
    bool m_connected = false;

public:
    ~BaiduVoteClient() { closeSocket(); }

    bool connect() {
        closeSocket();
        if (!m_socket.Create()) {
            ErrorHandler::fatalError("Socket创建失败");
        }
        m_connected = (m_socket.Connect(_T("wapp.baidu.com"), 80) != FALSE);
        return m_connected;
    }

    bool sendData(const std::string& data) {
        if (!m_connected) return false;
        const char* p = data.c_str();
        int total = static_cast<int>(data.length());
        int sent = 0;
        while (sent < total) {
            int n = m_socket.Send(p + sent, total - sent);
            if (n == SOCKET_ERROR || n == 0) return false;
            sent += n;
        }
        return true;
    }

    int receiveData(char* buffer, int bufferSize) {
        if (!m_connected) return SOCKET_ERROR;
        memset(buffer, 0, bufferSize);
        return m_socket.Receive(buffer, bufferSize - 1);
    }

    void closeSocket() {
        if (m_connected) {
            m_socket.Close();
            m_connected = false;
        }
    }
};

// ==============================
// 主程序
// ==============================
int main() {
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0) || !AfxSocketInit()) {
        std::cout << "MFC/Socket 初始化失败" << std::endl;
        return 1;
    }

    RequestBuilder reqBuilder;
    BaiduVoteClient client;
    char recvBuffer[15000] = { 0 };

    std::string address, selectItem;
    std::cout << "帖子地址ID: ";
    std::cin >> address;
    std::cout << "投票选项(数字): ";
    std::cin >> selectItem;

    reqBuilder.setAddress(address);
    reqBuilder.setSelectItem(selectItem);

    if (!client.connect()) {
        ErrorHandler::fatalError("服务器连接失败");
    }

    // 首次 GET，拿 sign_id / vote_id
    client.sendData(reqBuilder.buildGetRequest());
    int recvLen = client.receiveData(recvBuffer, sizeof(recvBuffer));
    if (recvLen == SOCKET_ERROR) {
        ErrorHandler::fatalError("首次请求失败");
    }
    {
        ResponseParser parser(recvBuffer);
        reqBuilder.setSignId(parser.findSignId());
        reqBuilder.setVoteId(parser.findVoteId());
    }

    // 循环投票
    std::cout << "开始循环投票..." << std::endl;
    while (true) {
        client.sendData(reqBuilder.buildGetRequest());
        recvLen = client.receiveData(recvBuffer, sizeof(recvBuffer));

        if (recvLen == 0 || recvLen == SOCKET_ERROR) {
            std::cout << "连接断开，正在重连..." << std::endl;
            client.closeSocket();
            if (!client.connect()) {
                std::cout << "重连失败，重试中..." << std::endl;
                Sleep(1000);
                continue;
            }
            continue;
        }

        if (recvBuffer[0] != 'H') continue;

        ResponseParser currParser(recvBuffer);
        std::string uid = currParser.extractBaiduWiseUid();
        if (uid.empty()) continue;

        reqBuilder.setBaiduWiseUid(uid);
        reqBuilder.setIsNewUser(currParser.extractIsNewUser());
        reqBuilder.setBaiduId(currParser.extractBaiduId());
        reqBuilder.setTbs(currParser.extractTbs());

        client.sendData(reqBuilder.buildPostRequest());
        client.receiveData(recvBuffer, sizeof(recvBuffer));

        std::cout << "投票请求已发送" << std::endl;
        Sleep(1000);
    }

    return 0;
}
