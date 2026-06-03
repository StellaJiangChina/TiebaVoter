//AI转换出来的C语言版本
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

// 全局缓冲区（对应汇编中的 szBuf）
char szBuf[15000] = {0};

// 错误处理函数（对应 sbErr）
void sbErr(HANDLE hOut, HANDLE hIn) {
    const char cErr[] = "Error!\r\nPress Any Key to Quit...";
    DWORD dwBytes;
    WriteConsole(hOut, cErr, sizeof(cErr) - 1, &dwBytes, NULL);
    ReadConsole(hIn, szBuf, 2, &dwBytes, NULL);
    ExitProcess(0);
}

int main() {
    // --------------------------
    // 1. 初始化控制台和句柄
    // --------------------------
    AllocConsole();
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwBytes;

    // --------------------------
    // 2. 定义 HTTP 请求头缓冲区（对应汇编中的 org 定位）
    // --------------------------
    char cHead1[0x37] = "GET /f/q---wiaui_1339034079_1644--1-1-0/m?kz=";
    char cFoot1[0x1B0 - 0x37] = "HTTP/1.1\r\nHost: wapp.baidu.com\r\nConnection: keep-alive\r\n"
                                  "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
                                  "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
                                  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                                  "Accept-Encoding: gzip,deflate,sdch\r\n"
                                  "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n";
    
    char cHead2[0x32D] = "POST /f/q---wiaui_1339034079_1644--1-1-0/m HTTP/1.1\r\n"
                          "Host: wapp.baidu.com\r\nConnection: keep-alive\r\nContent-Length: 198\r\n"
                          "Cache-Control: max-age=0\r\nOrigin: http://wapp.baidu.com\r\n"
                          "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
                          "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
                          "Content-Type: application/x-www-form-urlencoded\r\n";
    
    char cHead22[0x461 - 0x32D] = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                                    "Referer: http://wapp.baidu.com/f/q---wiaui_1339034079_1644--1-1-0/m?kz=0000000000\r\n"
                                    "Accept-Encoding: gzip,deflate,sdch\r\n"
                                    "Accept-Language: zh-CN,zh;q=0.8\r\n"
                                    "Cookie: BAIDU_WISE_UID=wapp_00000000000000000; IS_NEW_USER=";
    
    char cHead23[0x200] = "; BAIDUID=00000000000000000000000000000000:FG=1\r\n\r\n"
                           "select_items=00&pinf=&sign_id=00000000000000000000000000000000&can_post=&"
                           "vote_id=000000000000000000000000&product_id=1&tbs=00000000000000000000000000&"
                           "tn=bdVotPos&z=0000000000&sub1=%E6%8A%95%E7%A5%A8\r\n";

    // --------------------------
    // 3. 读取用户输入（Add> 和 Item>）
    // --------------------------
    const char cAdd[] = "Add>";
    WriteConsole(hOut, cAdd, sizeof(cAdd) - 1, &dwBytes, NULL);
    ReadConsole(hIn, cHead1 + 0x2D, 12, &dwBytes, NULL); // 输入写入 cHead1+0x2D

    // 处理地址（复制到 Referer、select_items 等位置）
    memcpy(cHead22 + 144, cHead1 + 0x2D, 12); // Referer
    memcpy(cHead23 + 215, cHead1 + 0x2D, 12); // select_items
    memcpy(cHead22 + 148, cHead1 + 0x31, 4);
    memcpy(cHead23 + 219, cHead1 + 0x31, 4);
    memcpy(cHead22 + 152, cHead1 + 0x35, 2);
    memcpy(cHead23 + 223, cHead1 + 0x35, 2);

    const char cItem[] = "Item>";
    WriteConsole(hOut, cItem, sizeof(cItem) - 1, &dwBytes, NULL);
    ReadConsole(hIn, cHead23 + 64, 4, &dwBytes, NULL); // 输入写入 cHead23+64
    *(WORD*)(cHead23 + 66) = 0x7026; // 对应汇编中的 mov word ptr cHead23+66,7026h

    // --------------------------
    // 4. 初始化 Winsock 并连接
    // --------------------------
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    SOCKET hSocket = socket(AF_INET, SOCK_STREAM, 0);
    const char cIP[] = "wapp.baidu.com";
    struct hostent* he = gethostbyname(cIP);
    if (he == NULL) sbErr(hOut, hIn);

    SOCKADDR_IN sockaddr_in = {0};
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = 0x5000; // 对应汇编中的 5000h
    sockaddr_in.sin_addr = *(struct in_addr*)he->h_addr_list[0];

    connect(hSocket, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in));

    // --------------------------
    // 5. 发送 GET 请求并接收响应
    // --------------------------
    char getReq[0x200] = {0};
    strcat(getReq, cHead1);
    strcat(getReq, cFoot1);
    send(hSocket, getReq, 411, 0);
    int recvLen = recv(hSocket, szBuf, 15000, 0);
    if (recvLen == SOCKET_ERROR) sbErr(hOut, hIn);

    // --------------------------
    // 6. 查找 "sign_" 字符串（对应 sbGetSignID）
    // --------------------------
    char* esi = szBuf + 2500;
    while (1) {
        esi++;
        if (*(DWORD*)esi == 0x5F6E6769) break; // 匹配 "ign_"（小端序）
    }

    // --------------------------
    // 7. 提取 SignID 和 VoteID（对应 Loadlpfixed）
    // --------------------------
    esi += 15;
    memcpy(cHead23 + 81, esi, 32);  // SignID（8个 dword）
    esi += 140;
    memcpy(cHead23 + 132, esi, 24); // VoteID（6个 dword）

    // --------------------------
    // 8. 循环发送请求（对应 sbVote，简化版）
    // --------------------------
    while (1) {
        // 发送 GET 请求
        send(hSocket, getReq, 411, 0);
        recvLen = recv(hSocket, szBuf, 15000, 0);
        if (recvLen == 0 || recvLen == -1) {
            closesocket(hSocket);
            hSocket = socket(AF_INET, SOCK_STREAM, 0);
            connect(hSocket, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in));
            continue;
        }
        if (szBuf[0] != 'H') continue;

        // 提取 BAIDU_WISE_UID
        if (szBuf[183] == 'D' && szBuf[207] == ';') {
            memcpy(cHead22 + 253, szBuf + 190, 21);
        } else continue;

        // 提取 IS_NEW_USER
        memcpy(cHead22 + 284, szBuf + 299, 24);

        // 提取 BAIDUID
        memcpy(cHead23 + 10, szBuf + 781, 32);

        // 提取 tbs
        char* tbsPtr = szBuf + 781 + 256;
        memcpy(cHead23 + 174, tbsPtr, 26);

        // 组装并发送 POST 请求
        char postReq[0x400] = {0};
        strcat(postReq, cHead2);
        strcat(postReq, cHead22);
        strcat(postReq, cHead23);
        send(hSocket, postReq, 940, 0);
        recv(hSocket, szBuf, 15000, 0);
    }

    return 0;
}
