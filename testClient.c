// 클라이언트 단

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
    char message[1460] = {0x00, };
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
    
    FILE *fp = fopen("size460.mp4", "wb");
    
    while((n = recv(clnt_sock, message, 1460, 0)) != 0)
    {
        fwrite(message, sizeof(char), n, fp);
        printf("Message from server : %x\n", n);
    }
    
    fclose(fp);
    
    // 사이즈 측정
    FILE *file = fopen("size460.mp4","rb");
    int fsize;
    
    fseek(file, 0, SEEK_END);
    fsize = ftell(file);
    
    printf("size460.mp4 size is %d\n", fsize);
    fclose(file);
    
    close(clnt_sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
