/*
파일명: 2016312644_client.c
작성자: 김태현
최종 수정일: 2020년 6월 6일
프로그램 설명: 소켓 프로그래밍을 이용한 대용량 동영상 파일 전송 (클라이언트 부분)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, const char *argv[])
{
  /* 변수 선언 */
  int clnt_sock;

  struct sockaddr_in serv_addr;

  char send_msg[] = "Hello, server!"; // 서버로 전달할 메시지
  char recv_msg[50];                  // 서버에게 받을 메시지 저장
  char buffer[1460];                  // 1460바이트씩 전달받을 버퍼

  int n;             // recv 리턴 값 저장
  int cnt = 0;       // 바이트 전달 횟수 저장
  int ori_file_size; // 원본 동영상 파일 크기
  int file_size;     // 받은 동영상 파일 크기
  char yes_or_no;    // y or n

  /* Step 1: 소켓 생성 */
  clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (clnt_sock == -1)
    perror("socket errer");

  /* Step 2: IP주소 및 포트번호 지정*/
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(atoi(argv[2]));

  /* Step 3. connect(): 서버에게 연결 요청 */
  if (connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    perror("connect error");

  /* Step 4. 연결이 완료되면 메시지("Hello, server!") 전송 */
  send(clnt_sock, send_msg, sizeof(send_msg), 0);

  /* 서버로부터 파일을 받을 건지 질문 수용 */
  recv(clnt_sock, recv_msg, sizeof(recv_msg), 0);
  printf("from server: %s. [y/n]", recv_msg);

  /* y or n의 답을 서버로 전달 */
  scanf("%c", &yes_or_no);
  send(clnt_sock, &yes_or_no, sizeof(yes_or_no), 0);

  printf("\n");

  /* Step 5. 파일 사이즈 받기(파일을 다 쓰고 난 후 비교용)*/
  recv(clnt_sock, &ori_file_size, sizeof(int), 0);

  /* 파일을 받겠다고 했을 때, 저장 및 쓰기 시작 */
  if (yes_or_no == 'Y' || yes_or_no == 'y')
  {
    /*--- 핵심: 파일 받고 쓰는 부분 ---*/
    FILE *fp = fopen("test.mp4", "wb");

    while ((n = recv(clnt_sock, buffer, 1460, 0)) != 0)
    {
      fwrite(buffer, sizeof(char), n, fp);
      cnt += 1;
      printf("from server : %4dbytes is downloading(:#%d)...\r", n, cnt);
    }

    /* 새로 쓴 파일 사이즈 측정 */
    file_size = ftell(fp);

    fclose(fp);
    /*--- // 파일 받고 쓰기 완료 ---*/

    /* 원본 파일의 사이즈와 비교해서 크기가 같으면 성공 메시지 출력, 아니라면 실패 메시지 출력 */
    if (file_size == ori_file_size)
      printf("\n\ncomplete: Total size is %d bytes.\n", file_size);
    else
      printf("\n\nfailed: size is different.\n");
  }
  /* 파일을 받지 않는다면 종료 */
  else
    printf("termination.\n");

  /* 소켓 닫기 */
  close(clnt_sock);
  return 0;
}
