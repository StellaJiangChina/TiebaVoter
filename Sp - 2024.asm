;2024-4-17 简单的重构了代码，但是已经丢失了最简单的版本，其实不需要发送这么完整的Cookie。当然，现在这个软件已经失效了。
;原理就是不停的发投票的数据包，使用的是wap版网页的协议。
;masm32编译通过 下载地址：https://masm32.com/
.386
.model flat,stdcall
option casemap:none
include	windows.inc
include	kernel32.inc
includelib	kernel32.lib
include	WSock32.inc
includelib 	WSock32.lib

Move_eax macro src, dest ;以eax为中介，将32位数据在内存间转移
    mov eax, src
    mov dest, eax
endm


.data
org 0h
cHead1                  db                  'GET /f/q---wiaui_1339034079_1644--1-1-0/m?kz='
org 37h
cFoot1                  db                  ' HTTP/1.1',0dh,0ah,'Host: wapp.baidu.com',0dh,0ah,'Connection: keep-alive',0dh,0ah, 'User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21',0dh,0ah,'Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3',0dh,0ah,'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',0dh,0ah,'Accept-Encoding: gzip,deflate,sdch',0dh,0ah,'Accept-Language: zh-CN,zh;q=0.8',0dh,0ah,0dh,0ah
org 1B0h
cHead2                  db                  'POST /f/q---wiaui_1339034079_1644--1-1-0/m HTTP/1.1',0dh,0ah,'Host: wapp.baidu.com',0dh,0ah,'Connection: keep-alive',0dh,0ah,'Content-Length: 198',0dh,0ah,'Cache-Control: max-age=0',0dh,0ah,'Origin: http://wapp.baidu.com',0dh,0ah,'User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1041.0 Safari/535.21',0dh,0ah,'Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3',0dh,0ah,'Content-Type: application/x-www-form-urlencoded',0dh,0ah
org 32Dh
cHead22                 db                  'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',0dh,0ah,\
                                            'Referer: http://wapp.baidu.com/f/q---wiaui_1339034079_1644--1-1-0/m?kz=0000000000',0dh,0ah,'Accept-Encoding: gzip,deflate,sdch',0dh,0ah,\
                                            'Accept-Language: zh-CN,zh;q=0.8',0dh,0ah,'Cookie: BAIDU_WISE_UID=wapp_00000000000000000; IS_NEW_USER='
org 461h
cHead23                 db                  '; BAIDUID=00000000000000000000000000000000:FG=1',0dh,0ah,0dh,0ah,'select_items=00&pinf=&sign_id=00000000000000000000000000000000&can_post=&vote_id=000000000000000000000000&product_id=1&tbs=00000000000000000000000000&tn=bdVotPos&z=0000000000&sub1=%E6%8A%95%E7%A5%A8',0dh,0ah
SOCKADDR_IN				sockaddr_in			<AF_INET,5000h,,>
.data?
szBuf					db					15000 dup(?)
hIn						dd					?
hOut					dd					?
dwBytes					dd					?
hSocket					dd					?
WSAData					WSADATA 		 	<>
.const	
cAdd					db					'Add>'
cIP 					db					'wapp.baidu.com',0
cItem					db					'Item>'
cErr                    db                  'Error!',0dh,0ah,'Press Any Key to Quit...'
.code

sbErr: ;出错后调用的过程，我也不知道当时为什么不用sub，要用sb。现在看来真的好笑。
    invoke WriteConsole,hOut,offset cErr,sizeof cErr,offset dwBytes,0;控制台输出cErr的内容，即”Error！Press Any Key to Quit...“
    invoke ReadConsole,hIn,offset szBuf,2,offset dwBytes,0;读取任意键
    invoke ExitProcess,0;退出程序
sbVote:
    invoke send,hSocket,offset cHead1,411,0;411刚好是cHead1和cFoot1合并的长度。
    invoke recv,hSocket,offset szBuf,15000,0
    .if eax == 0 || eax == -1       ;if Error Or Zero Size Packet
    invoke closesocket,hSocket
    invoke	socket,AF_INET,SOCK_STREAM,0
	mov		hSocket,eax
	invoke  connect,hSocket,offset SOCKADDR_IN,sizeof SOCKADDR_IN
    jmp sbVote
    .endif
    cmp byte ptr szBuf,48h          ;if First letter of the packet "H"
    jne sbVote
;---------Load BAIDU_WISE_UID-----------------------
    mov al,byte ptr szBuf + 183d
    mov ah,byte ptr szBuf + 207d
    .if al == 44h && ah == 3bh
    i = 0
    while i LT 4;while LT是宏，归纳重复的代码
    Move_eax dword ptr szBuf + 190 + i * 4, dword ptr cHead22 + 253 + i * 4
    i = i + 1
    endm
        mov al ,byte ptr szBuf + 206
        mov  byte ptr cHead22 + 269,al
    .else
        jmp sbVote
    .endif
;-----------Load BAIDU_WISE_UID-----------------------

;-----------写入IS_NEW_USER到cHead22的对应位置------------------------
    i = 0;while LT是宏，归纳重复的代码
    while i LT 6
    Move_eax dword ptr szBuf + 299 + i * 4, dword ptr cHead22 + 284 + i * 4
    i = i + 1
    endm
;-------------写入IS_NEW_USER到cHead22的对应位置---------------------------

;------------------写入BaiduID至cHead23的对应位置-------------------------
    i = 0
    while i LT 8;while LT是宏，归纳重复的代码
    Move_eax dword ptr szBuf + 781 + i * 4, dword ptr cHead23 + 10 + i * 4
    i = i + 1
    endm
;------------------写入BaiduID至cHead23的对应位置-------------------------

;-----------------Load tbs---------------------
    i = 0
    while i LT 6
    Move_eax [esi + 256 + i *4], dword ptr cHead23 + 174 + i * 4
    i = i + 1
    endm
        mov ax, [esi + 280]
        mov word ptr cHead23 +  198,ax
;-------------------Load tbs--------------------
        invoke send,hSocket,offset cHead2,940,0
        invoke recv,hSocket,offset szBuf,15000,0
        jmp sbVote
Loadlpfixed:
;---------------------Load SignID----------------------------
    i = 0
    while i LT 8
    Move_eax [esi + 15 + i *4], dword ptr cHead23 + 81 + i * 4
    i = i + 1
    endm
;---------------Load SignID------------------------

;---------------Load VoteID------------------------
    i = 0
    while i LT 8
    Move_eax [esi + 140 + i *4], dword ptr cHead23 + 132 + i * 4
    i = i + 1
    endm
;-------------Load VoteID---------------------------
    jmp     sbVote
sbGetSignID:
    inc esi
    mov eax,[esi]
    cmp eax,5f6e6769h
    je  Loadlpfixed
    jne sbGetSignID
Start:
    invoke  AllocConsole
    invoke  GetStdHandle,STD_INPUT_HANDLE
	mov		hIn,eax
	invoke  GetStdHandle,STD_OUTPUT_HANDLE
	mov		hOut,eax
	invoke  WriteConsole,hOut,offset cAdd,4d,offset dwBytes,0;输出'Add>'
	invoke	ReadConsole,hIn,offset cHead1 + 2dh,12,offset dwBytes,0;将贴子编号写入cHead1的对应位置
;------------------------------------------------------写入贴子编号到需要的位置 
	mov     word ptr cFoot1,4820h       ;Change CRLR into Correct，左边的英文是当时写的注释，现在也看不懂了。
	mov     eax,dword ptr cHead1 + 2dh
    mov     dword ptr cHead22 + 144,eax ;Put it into Referer，左边的英文是当时写的注释，现在也看不懂了。
    mov     dword ptr cHead23 + 215,eax ;Put it into select_items，左边的英文是当时写的注释，现在也看不懂了。
	Move_eax dword ptr cHead1 + 31h, dword ptr cHead22 + 148
    mov     dword ptr cHead23 + 219,eax
    mov      ax,word ptr cHead1 + 35h
    mov     word ptr cHead22 + 152,ax
    mov     word ptr cHead23 + 223,ax
;------------------------------------------------------写入贴子编号到需要的位置 
	invoke	WriteConsole,hOut,offset cItem,5d,offset dwBytes,0
	invoke 	ReadConsole,hIn,offset cHead23 + 64,4d,offset dwBytes,0	;Read Item  Format:03or14
    mov     word ptr cHead23 + 66,7026h 
    invoke  WSAStartup,257d,offset szBuf
	invoke  socket,AF_INET,SOCK_STREAM,0
	mov		hSocket,eax
    invoke  gethostbyname,offset cIP
	add     eax,12d
    mov     eax,[eax]
    mov     eax,[eax]
    mov     eax,[eax]
	mov 	SOCKADDR_IN.sin_addr,eax	;Read IP
	invoke  connect,hSocket,offset SOCKADDR_IN,sizeof SOCKADDR_IN
    invoke  send,hSocket,offset cHead1,411,0
    invoke  recv,hSocket,offset szBuf,15000,0
    cmp     eax,SOCKET_ERROR
    je      sbErr
    mov     esi,offset szBuf + 2500d
    jmp     sbGetSignID
end Start

