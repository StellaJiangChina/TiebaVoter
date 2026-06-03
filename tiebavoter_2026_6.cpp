//AI生成的C++版本
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#pragma comment(lib, "ws2_32.lib")

// ==============================
// 错误处理类：统一错误输出与退出
// ==============================
class ErrorHandler {
public:
    static void fatalError(const std::string& msg) {
        std::cout << "[错误] " << msg << std::endl;
        std::cout << "按任意键退出..." << std::endl;
        char temp;
        std::cin.get(temp);
        exit(EXIT_FAILURE);
    }
};

// ==============================
// 请求构建类：组装GET/POST HTTP请求
// ==============================
class RequestBuilder {
private:
    // 固定请求头片段（与原代码完全一致）
    const std::string GET_HEAD = "GET /f/q---wiaui_1339034079_1644--1-1-0/m?kz=";
    const std::string GET_FOOT = " HTTP/1.1\r\nHost: wapp.baidu.com\r\nConnection: keep-alive\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
        "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Accept-Encoding: gzip,deflate,sdch\r\n"
        "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n";

    const std::string POST_HEAD = "POST /f/q---wiaui_1339034079_1644--1-1-0/m HTTP/1.1\r\n"
        "Host: wapp.baidu.com\r\nConnection: keep-alive\r\nContent-Length: 198\r\n"
        "Cache-Control: max-age=0\r\nOrigin: http://wapp.baidu.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
        "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Referer: http://wapp.baidu.com/f/q---wiaui_1339034079_1644--1-1-0/m?kz=";

    const std::string POST_PART2 = "\r\nAccept-Encoding: gzip,deflate,sdch\r\n"
        "Accept-Language: zh-CN,zh;q=0.8\r\nCookie: BAIDU_WISE_UID=wapp_";
    const std::string POST_PART3 = "; IS_NEW_USER=";
    const std::string POST_PART4 = "; BAIDUID=";
    const std::string POST_PART5 = ":FG=1\r\n\r\nselect_items=";
    const std::string POST_PART6 = "&pinf=&sign_id=";
    const std::string POST_PART7 = "&can_post=&vote_id=";
    const std::string POST_PART8 = "&product_id=1&tbs=";
    const std::string POST_PART9 = "&tn=bdVotPos&z=0000000000&sub1=%E6%8A%95%E7%A5%A8\r\n";

    // 动态参数
    std::string m_address;       // 帖子ID
    std::string m_selectItem;    // 投票选项
    std::string m_baiduWiseUid;  // Cookie参数
    std::string m_isNewUser;     // Cookie参数
    std::string m_baiduId;       // Cookie参数
    std::string m_signId;        // 签名ID
    std::string m_voteId;        // 投票ID
    std::string m_tbs;           // 验证参数

public:
    // 设置动态参数
    void setAddress(const std::string& addr) { m_address = addr; }
    void setSelectItem(const std::string& item) { m_selectItem = item; }
    void setBaiduWiseUid(const std::string& uid) { m_baiduWiseUid = uid; }
    void setIsNewUser(const std::string& user) { m_isNewUser = user; }
    void setBaiduId(const std::string& id) { m_baiduId = id; }
    void setSignId(const std::string& sign) { m_signId = sign; }
    void setVoteId(const std::string& vote) { m_voteId = vote; }
    void setTbs(const std::string& tbs) { m_tbs = tbs; }

    // 构建完整GET请求
    std::string buildGetRequest() const {
        return GET_HEAD + m_address + GET_FOOT;
    }

    // 构建完整POST请求
    std::string buildPostRequest() const {
        return POST_HEAD + m_address + POST_PART2 + m_baiduWiseUid + POST_PART3 + m_isNewUser
            + POST_PART4 + m_baiduId + POST_PART5 + m_selectItem + POST_PART6 + m_signId
            + POST_PART7 + m_voteId + POST_PART8 + m_tbs + POST_PART9;
    }
};

// ==============================
// 响应解析类：从服务器返回数据中提取参数
// ==============================
class ResponseParser {
private:
    char* m_buffer;
    int m_bufferSize;

public:
    ResponseParser(char* buffer, int size) : m_buffer(buffer), m_bufferSize(size) {}

    // 查找sign_id（原代码小端匹配逻辑完全保留）
    std::string findSignId() {
        char* strPtr = m_buffer + 2500;
        while (strPtr < m_buffer + m_bufferSize - 4) {
            strPtr++;
            // 小端序匹配 "ign_" (0x69 0x67 0x6E 0x5F)
            if (*(DWORD*)strPtr == 0x5F6E6769) {
                break;
            }
        }
        if (strPtr >= m_buffer + m_bufferSize - 15 - 32) {
            ErrorHandler::fatalError("未找到sign_id");
        }
        return std::string(strPtr + 15, 32);
    }

    // 提取vote_id
    std::string findVoteId(const char* signIdPtr) {
        return std::string(signIdPtr + 140, 24);
    }

    // 提取BAIDU_WISE_UID（原代码偏移量完全保留）
    std::string extractBaiduWiseUid() {
        if (m_buffer[183] == 'D' && m_buffer[207] == ';') {
            return std::string(m_buffer + 190, 21);
        }
        return "";
    }

    // 提取IS_NEW_USER
    std::string extractIsNewUser() {
        return std::string(m_buffer + 299, 24);
    }

    // 提取BAIDUID
    std::string extractBaiduId() {
        return std::string(m_buffer + 781, 32);
    }

    // 提取tbs参数
    std::string extractTbs() {
        return std::string(m_buffer + 781 + 256, 26);
    }
};

// ==============================
// 投票客户端类：封装所有网络操作
// ==============================
class BaiduVoteClient {
private:
    SOCKET m_hSocket;
    SOCKADDR_IN m_serverAddr;
    const std::string m_serverHost = "wapp.baidu.com";
    const int m_serverPort = 80; // 0x5000

    // 初始化Winsock
    void initWinsock() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            ErrorHandler::fatalError("Winsock初始化失败");
        }
    }

    // 解析域名
    void resolveHost() {
        struct hostent* he = gethostbyname(m_serverHost.c_str());
        if (he == NULL) {
            ErrorHandler::fatalError("域名解析失败");
        }
        m_serverAddr.sin_family = AF_INET;
        m_serverAddr.sin_port = htons(m_serverPort);
        m_serverAddr.sin_addr = *(struct in_addr*)he->h_addr_list[0];
    }

public:
    BaiduVoteClient() {
        initWinsock();
        resolveHost();
        m_hSocket = INVALID_SOCKET;
    }

    ~BaiduVoteClient() {
        closeSocket();
        WSACleanup();
    }

    // 连接服务器
    bool connect() {
        closeSocket();
        m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_hSocket == INVALID_SOCKET) {
            ErrorHandler::fatalError("Socket创建失败");
        }
        return ::connect(m_hSocket, (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) != SOCKET_ERROR;
    }

    // 发送数据
    int sendData(const std::string& data) {
        return send(m_hSocket, data.c_str(), data.length(), 0);
    }

    // 接收数据
    int receiveData(char* buffer, int bufferSize) {
        memset(buffer, 0, bufferSize);
        return recv(m_hSocket, buffer, bufferSize - 1, 0);
    }

    // 关闭Socket
    void closeSocket() {
        if (m_hSocket != INVALID_SOCKET) {
            closesocket(m_hSocket);
            m_hSocket = INVALID_SOCKET;
        }
    }
};

// ==============================
// 主程序入口
// ==============================
int main() {
    // 1. 初始化组件
    RequestBuilder reqBuilder;
    BaiduVoteClient client;
    char recvBuffer[15000] = { 0 };

    // 2. 输入用户参数
    std::string address, selectItem;
    std::cout << "帖子地址ID: ";
    std::cin >> address;
    std::cout << "投票选项(数字): ";
    std::cin >> selectItem;

    reqBuilder.setAddress(address);
    reqBuilder.setSelectItem(selectItem);

    // 3. 连接服务器
    if (!client.connect()) {
        ErrorHandler::fatalError("服务器连接失败");
    }

    // 4. 首次GET请求获取sign_id和vote_id
    std::string getReq = reqBuilder.buildGetRequest();
    client.sendData(getReq);
    int recvLen = client.receiveData(recvBuffer, sizeof(recvBuffer));
    if (recvLen == SOCKET_ERROR) {
        ErrorHandler::fatalError("首次请求失败");
    }

    // 5. 解析初始响应
    ResponseParser parser(recvBuffer, recvLen);
    std::string signId = parser.findSignId();
    std::string voteId = parser.findVoteId(recvBuffer + 2500 + 15);
    reqBuilder.setSignId(signId);
    reqBuilder.setVoteId(voteId);

    // 6. 循环投票主逻辑
    std::cout << "开始循环投票..." << std::endl;
    while (true) {
        // 发送GET请求获取最新Cookie
        getReq = reqBuilder.buildGetRequest();
        client.sendData(getReq);
        recvLen = client.receiveData(recvBuffer, sizeof(recvBuffer));

        // 连接断开自动重连
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

        // 跳过无效响应
        if (recvBuffer[0] != 'H') {
            continue;
        }

        // 提取Cookie参数
        ResponseParser currParser(recvBuffer, recvLen);
        std::string uid = currParser.extractBaiduWiseUid();
        if (uid.empty()) {
            continue;
        }

        reqBuilder.setBaiduWiseUid(uid);
        reqBuilder.setIsNewUser(currParser.extractIsNewUser());
        reqBuilder.setBaiduId(currParser.extractBaiduId());
        reqBuilder.setTbs(currParser.extractTbs());

        // 构建并发送POST投票请求
        std::string postReq = reqBuilder.buildPostRequest();
        client.sendData(postReq);
        client.receiveData(recvBuffer, sizeof(recvBuffer));

        std::cout << "投票请求已发送" << std::endl;
        Sleep(1000); // 防止请求过快
    }

    return 0;
}
