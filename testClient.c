//
//  main.c
//  test_clnt
//
//  Created by taehy.k on 2020/05/29.
//  Copyright © 2020 taehy.k. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char* message);

int main(int argc, const char * argv[]) {
    // insert code here...
    int clnt_sock;
    struct sockaddr_in serv_addr;
    char test_msg[] = "Hello, server!";
    char message[1024] = {0x00, };
    char buffer[20];
    int n;
    
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_sock == -1)
        error_handling("socket errer");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    if (connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect error");
    
    
    scanf("%s", buffer);
    buffer[strlen(buffer)] = '\0';
    write(clnt_sock, buffer, strlen(buffer)+1);
    n = read(clnt_sock, message, sizeof(message)-1);
    if (n < 0)
        return -1;
    
    //write(clnt_sock, test_msg, sizeof(test_msg)+1);
    printf("Message from server : %s\n", message);
    close(clnt_sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
