//
//  main.c
//  test
//
//  Created by taehy.k on 2020/05/29.
//  Copyright © 2020 taehy.k. All rights reserved.
//
//  소켓 통신 서버 프로그램의 이해 200529

// int socket(int domain, int type, int protocol);


#include <stdio.h>
#include <stdlib.h> // atoi 사용
#include <string.h> // memset
#include <unistd.h> // sockaddr_in, read, write
#include <sys/socket.h>
#include <arpa/inet.h> // htonl, htons, INADDR_ANY, sockaddr_in

#define BUF_LEN 128
// 메시지 송수신에 사용될 버퍼 크기를 선언

// 데이터 전송!!!
char msg[] = "Hello this is server!\n";
void error_handling(char *message);

int main(int argc, char *argv[])
{
    char buffer[BUF_LEN];
    
    int serv_sock;
    int clnt_sock;
    
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr; //accept에서 사용
    socklen_t clnt_addr_size;
    
    char temp[20];
    int len, msg_size;
    
    
    /* step 1. 소켓 생성 */
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    // int domain: PF_INET 사용 (IPv4 사용)
    // int type: TCP or UDP 선택 SOCKET_STREAM(TCP)
    // int protocol: IPPROTO_TCP(TCP방식), type에서 미리 정해진 경우 0
    if(serv_sock == -1)
        error_handling("socket error");

    // 리턴 값: 소켓 디스크립터 반환
    // -1: 소켓 생성 실패
    // 0 이상의 값: 소켓 디스크립터
    
    // 주소 초기화 및 IP주소, 포트 지정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // IP 주소
    serv_addr.sin_port = htons(atoi(argv[1])); // 포트 대입
    
    /* step 2. bind() */
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind error");
    // int sockfd: 소켓 디스크립터 대입, struct sockaddr *myaddr: 서버 IP주소, socklen_t addrlen: 주소 길이
    // 성공 시 0, 실패 시 -1
    
    /* step 3. listen() */ // 연결 대기열 5개 생성
    if(listen(serv_sock, 5) == -1)
        error_handling("listen error");
    
    printf("현재 서버가 열려있는 중입니다.. 기다려주세요...\n");
    
    // 해당 코드 부분이 중요
    while(1){
        // accpet부분
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1)
            error_handling("accept error");
        inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, temp, sizeof(temp));
        
        msg_size = read(clnt_sock, buffer, 1024);
        write(clnt_sock, buffer, msg_size);
        
        close(clnt_sock);
        printf("Server : %s client closed.\n", temp);
    }
    
    
    // 소켓 닫기
    
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
