;Compile with the masm32 https://masm32.com/
.386
.model flat,stdcall
option casemap:none
include		windows.inc
include		kernel32.inc
includelib	kernel32.lib
include		WSock32.inc
includelib 	WSock32.lib
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

sbErr:
    invoke WriteConsole,hOut,offset cErr,sizeof cErr,offset dwBytes,0
    invoke ReadConsole,hIn,offset szBuf,2,offset dwBytes,0
    invoke ExitProcess,0
sbVote:
    invoke send,hSocket,offset cHead1,411,0
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
        mov eax,dword ptr szBuf + 190
        mov dword ptr cHead22 + 253,eax
        mov eax,dword ptr szBuf + 194
        mov dword ptr cHead22 + 257,eax
        mov eax,dword ptr szBuf + 198
        mov dword ptr cHead22 + 261,eax
        mov eax,dword ptr szBuf + 202
        mov dword ptr cHead22 + 265,eax
        mov al ,byte ptr szBuf + 206
        mov  byte ptr cHead22 + 269,al
    .else
        jmp sbVote
    .endif
;-----------Load BAIDU_WISE_UID-----------------------

;-----------Load IS_NEW_USER-------------------------
        mov eax,dword ptr szBuf + 299
        mov dword ptr cHead22 + 284,eax
        mov eax,dword ptr szBuf + 303
        mov dword ptr cHead22 + 288,eax
        mov eax,dword ptr szBuf + 307
        mov dword ptr cHead22 + 292,eax
        mov eax,dword ptr szBuf + 311
        mov dword ptr cHead22 + 296,eax
        mov eax,dword ptr szBuf + 315
        mov dword ptr cHead22 + 300,eax
        mov eax,dword ptr szBuf + 319
        mov dword ptr cHead22 + 304,eax
;-------------Load IS_NEW_USER---------------------------

;------------------Load BaiduID-------------------------
        mov eax,dword ptr szBuf + 781
        mov dword ptr cHead23 + 10,eax
        mov eax,dword ptr szBuf + 785
        mov dword ptr cHead23 + 14,eax
        mov eax,dword ptr szBuf + 789
        mov dword ptr cHead23 + 18,eax
        mov eax,dword ptr szBuf + 793
        mov dword ptr cHead23 + 22,eax
        mov eax,dword ptr szBuf + 797
        mov dword ptr cHead23 + 26,eax
        mov eax,dword ptr szBuf + 801
        mov dword ptr cHead23 + 30,eax
        mov eax,dword ptr szBuf + 805
        mov dword ptr cHead23 + 34,eax
        mov eax,dword ptr szBuf + 809
        mov dword ptr cHead23 + 38,eax
;----------------Load BaiduID----------------------------

;-----------------Load tbs---------------------
        mov eax,[esi + 256]
        mov dword ptr cHead23 + 174,eax
        mov eax,[esi + 260]
        mov dword ptr cHead23 + 178,eax
        mov eax,[esi + 264]
        mov dword ptr cHead23 + 182,eax
        mov eax,[esi + 268]
        mov dword ptr cHead23 + 186,eax
        mov eax,[esi + 272]
        mov dword ptr cHead23 + 190,eax
        mov eax,[esi + 276]
        mov dword ptr cHead23 + 194,eax
        mov ax, [esi + 280]
        mov word ptr cHead23 +  198,ax
;-------------------Load tbs--------------------
        invoke send,hSocket,offset cHead2,940,0
        invoke recv,hSocket,offset szBuf,15000,0
        jmp sbVote
Loadlpfixed:
;---------------------Load SignID----------------------------
    mov     eax,[esi + 15]
    mov     dword ptr cHead23 + 81,eax
    mov     eax,[esi + 19]
    mov     dword ptr cHead23 + 85,eax
    mov     eax,[esi + 23]
    mov     dword ptr cHead23 + 89,eax
    mov     eax,[esi + 27]
    mov     dword ptr cHead23 + 93,eax
    mov     eax,[esi + 31]
    mov     dword ptr cHead23 + 97,eax
    mov     eax,[esi + 35]
    mov     dword ptr cHead23 + 101,eax
    mov     eax,[esi + 39]
    mov     dword ptr cHead23 + 105,eax
    mov     eax,[esi + 43]
    mov     dword ptr cHead23 + 109,eax
;---------------Load SignID------------------------

;---------------Load VoteID------------------------
    mov     eax,[esi + 140]             
    mov     dword ptr cHead23 + 132,eax
    mov     eax,[esi + 144]            
    mov     dword ptr cHead23 + 136,eax
    mov     eax,[esi + 148]            
    mov     dword ptr cHead23 + 140,eax
    mov     eax,[esi + 152]            
    mov     dword ptr cHead23 + 144,eax
    mov     eax,[esi + 156]            
    mov     dword ptr cHead23 + 148,eax
    mov     eax,[esi + 160]            
    mov     dword ptr cHead23 + 152,eax 
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
	invoke  WriteConsole,hOut,offset cAdd,4d,offset dwBytes,0
	invoke	ReadConsole,hIn,offset cHead1 + 2dh,12,offset dwBytes,0
;------------------------------------------------------Deal with Addres 
	mov     word ptr cFoot1,4820h       ;Change CRLR into Correct
	mov     eax,dword ptr cHead1 + 2dh
    mov     dword ptr cHead22 + 144,eax ;Put it into Referer
    mov     dword ptr cHead23 + 215,eax ;Put it into select_items
    mov     eax,dword ptr cHead1 + 31h
    mov     dword ptr cHead22 + 148,eax
    mov     dword ptr cHead23 + 219,eax
    mov      ax,word ptr cHead1 + 35h
    mov     word ptr cHead22 + 152,ax
    mov     word ptr cHead23 + 223,ax
;------------------------------------------------------Deal with Addres 
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

