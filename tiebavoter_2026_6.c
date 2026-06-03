//人工编写的C语言版本
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string.h>
#include <stdio.h>   // 提供printf、scanf
#include <stdlib.h>  // 提供exit函数
#pragma comment(lib, "ws2_32.lib")

struct GET_Pocket
{
    const char strHead[46] = "GET /f/q---wiaui_1339034079_1644--1-1-0/m?kz=";
    char Addres[12] = { 0 };
    const char strFoot[356] = "HTTP/1.1\r\nHost: wapp.baidu.com\r\nConnection: keep-alive\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
        "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Accept-Encoding: gzip,deflate,sdch\r\n"
        "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n";
};

struct POST_Pocket
{
    const char str1[527] = "POST /f/q---wiaui_1339034079_1644--1-1-0/m HTTP/1.1\r\n"
        "Host: wapp.baidu.com\r\nConnection: keep-alive\r\nContent-Length: 198\r\n"
        "Cache-Control: max-age=0\r\nOrigin: http://wapp.baidu.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21\r\n"
        "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Referer: http://wapp.baidu.com/f/q---wiaui_1339034079_1644--1-1-0/m?kz=;";
    char Addres[12] = { 0 };
    const char str2[100] =
        "\r\n"
        "Accept-Encoding: gzip,deflate,sdch\r\n"
        "Accept-Language: zh-CN,zh;q=0.8\r\n"
        "Cookie: BAIDU_WISE_UID=wapp_";
    char BAIDU_WISE_UID[25] = { 0 };
    const char str3[25] = "; IS_NEW_USER=";
    char IS_NEW_USER[25] = { 0 };
    const char str4[11] = "; BAIDUID=";
    char BAIDUID[30] = { 0 };
    const char str5[23] = ":FG=1\r\n\r\n"
         "select_items=";
    char select_items[3] = { 0 };
    const char str6[135] = "&pinf=&sign_id=0000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    char sign_id[32] = {0};
    const char str7[20] = "&can_post=&vote_id=";
    char vote_id[30] = { 0 };
    const char str8[19]  =  "&product_id=1&tbs=";
    char tbs[30] = { 0 };
    const char str9[52] = "&tn=bdVotPos&z=0000000000&sub1=%E6%8A%95%E7%A5%A8\r\n";
};
void Error(){
    printf("Error!\r\nPress Any Key to Quit...");
    char tempChar;
    scanf_s("%c", &tempChar);
    exit(EXIT_SUCCESS);
}

int main() {

    printf("地址:");
    struct GET_Pocket get_pocket;
    struct POST_Pocket post_pocket;
    scanf_s("%c", &post_pocket.Addres);
    strcpy_s(get_pocket.Addres, post_pocket.Addres);
    printf("选项:");
    scanf_s("%c", &post_pocket.select_items);

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    SOCKET hSocket = socket(AF_INET, SOCK_STREAM, 0);
    const char cIP[15] = "wapp.baidu.com";
    struct hostent* he = gethostbyname(cIP);
    if (he == NULL) Error;

    SOCKADDR_IN sockaddr_in = { 0 };
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = 0x5000; // 对应汇编中的 5000h
    sockaddr_in.sin_addr = *(struct in_addr*)he->h_addr_list[0];

    connect(hSocket, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in));

    // 5. 发送 GET 请求并接收响应
    char getReq[0x200] = { 0 };
    strcat_s(getReq, get_pocket.strHead);
    strcat_s(getReq, get_pocket.Addres);
    strcat_s(getReq, get_pocket.strFoot);
    send(hSocket, getReq, 411, 0);
    char szBuf[15000] = { 0 };
    int recvLen = recv(hSocket, szBuf, 15000, 0);
    if (recvLen == SOCKET_ERROR) Error;

    //查找 "sign_" 字符串
    char* strPtr = szBuf + 2500;
    while (1) {
        strPtr++;
        if (*(DWORD*)strPtr == 0x5F6E6769) break; // 匹配 "ign_"（小端序）
    }


    // 提取 SignID 和 VoteID
    strPtr += 15;
    memcpy(post_pocket.sign_id, strPtr, 32);  // SignID（8个 dword）
    strPtr += 140;
    memcpy(post_pocket.vote_id, strPtr, 24); // VoteID（6个 dword）

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

        if (szBuf[183] == 'D' && szBuf[207] == ';') {
            memcpy(post_pocket.BAIDU_WISE_UID, szBuf + 190, 21);
        }
        else continue;

        memcpy(post_pocket.IS_NEW_USER, szBuf + 299, 24);
        memcpy(post_pocket.BAIDUID, szBuf + 781, 32);
        char* tbsPtr = szBuf + 781 + 256;
        memcpy(post_pocket.tbs, tbsPtr, 26);

        // 组装并发送 POST 请求
        char postReq[0x400] = { 0 };
        strcat_s(postReq, post_pocket.str1);
        strcat_s(postReq, post_pocket.Addres);
        strcat_s(postReq, post_pocket.str2);
        strcat_s(postReq, post_pocket.BAIDU_WISE_UID);
        strcat_s(postReq, post_pocket.str3);
        strcat_s(postReq, post_pocket.IS_NEW_USER);
        strcat_s(postReq, post_pocket.str4);
        strcat_s(postReq, post_pocket.BAIDUID);
        strcat_s(postReq, post_pocket.str5);
        strcat_s(postReq, post_pocket.select_items);
        strcat_s(postReq, post_pocket.str6);
        strcat_s(postReq, post_pocket.sign_id);
        strcat_s(postReq, post_pocket.str7);
        strcat_s(postReq, post_pocket.vote_id);
        strcat_s(postReq, post_pocket.str8);
        strcat_s(postReq, post_pocket.tbs);
        strcat_s(postReq, post_pocket.str9);

        send(hSocket, postReq, 940, 0);
        recv(hSocket, szBuf, 15000, 0);
    }

    return 0;
}
