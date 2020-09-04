/*
파일명: 2016312644_server.c
작성자: 김태현
최종 수정일: 2020년 6월 6일
프로그램 설명: 소켓 프로그래밍을 이용한 대용량 동영상 파일 전송 (서버 부분)
*/

#include <stdio.h>
#include <stdlib.h> // atoi 사용
#include <string.h> // memset
#include <unistd.h> // sockaddr_in, read, write
#include <sys/socket.h>
#include <arpa/inet.h> // htonl, htons, INADDR_ANY, sockaddr_in

int main(int argc, char *argv[])
{
  /* 변수 선언 */
  int serv_sock; // 서버 소켓
  int clnt_sock; // 클라이언트 소켓

  struct sockaddr_in serv_addr; // 구조체 (IP주소, 포트번호 세팅)
  struct sockaddr_in clnt_addr; // 구조체 (IP주소, 포트번호 세팅)
  socklen_t clnt_addr_size;

  char recv_msg[20];
  char send_msg[] = "Do you want to download the file? :test.mp4.";
  char yes_or_no;
  int len, msg_size;
  /* //변수 선언 마무리 */

  /* Step 1: 소켓 생성 */
  serv_sock = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성 (IPv4 도메인, TCP 통신)
  if (serv_sock == -1)                         // 소켓 생성 실패 시 에러메시지 출력 (-1인 경우 소켓 생성 실패)
    perror("socket error!");

  /* Step 2: IP주소 및 포트번호 지정*/
  memset(&serv_addr, 0, sizeof(serv_addr));      // serv_addr 구조체에 있는 값 초기화
  serv_addr.sin_family = AF_INET;                // IPv4 주소 체계 (위에서 나오는 PF_INET과 같은 의미로 보임.)
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 호스트-네트워크(long타입: 주소), INADDR_ANY: IP주소 자동 할당
  serv_addr.sin_port = htons(atoi(argv[1]));     // 호스트-네트워크(short타입: 포트), 명령어 입력 시 첫번째 인자가 포트번호로 들어옴

  /* Step 3. bind(): 소켓과 주소 연결 */
  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) // bind 실패 시 에러메시지 출력 (-1인 경우 bind 실패)
    perror("bind error");

  /* Step 4. listen(): 클라이언트 접속 대기 상태 */
  /* listen함수의 두번째 매개변수인 backlog부분의 역할을 정확히 이해하지 못하겠음.
       5라고 설정하면 클라이언트의 최대 접속 가능 수를 5개로 제한한다는 의미인 줄 알았으나,
       실제로 그렇지 않다고 함. (why?) */
  if (listen(serv_sock, 1) == -1) // listen 실패 시 에러메시지 출력 (-1인 경우 listen 실패)
    perror("listen error");

  printf("Starting server...\nQuit the server with CTRL-C.\n");
  printf("\n");

  /* Step 5. accept(): 클라이언트의 요청(connect)가 들어오면 연결을 허용 */
  clnt_addr_size = sizeof(clnt_addr);
  clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size); // 서버 소켓에 클라이언트 정보 연결
  if (clnt_sock == -1)                                                           // accpet 실패 시 에러메시지 출력 (-1인 경우 accpet 실패)
    perror("accept error");

  /* 클라이언트로 부터 hello, server 메시지를 전달받음. */
  recv(clnt_sock, recv_msg, sizeof(recv_msg), 0);
  printf("message from client: %s\n", recv_msg);

  /* 동영상 파일을 전달 받을 건지 클라이언트 측에 물어봄. */
  send(clnt_sock, send_msg, sizeof(send_msg), 0);

  /* 동영상 전달 여부를 클라이언트로 부터 받음. (y or n) */
  recv(clnt_sock, &yes_or_no, sizeof(yes_or_no), 0);

  /* Y(y)라는 답변이 클라이언트로부터 오면 파일 전송 시작 */
  if (yes_or_no == 'Y' || yes_or_no == 'y')
  {
    /*--- 실제 직접 구현해야할 부분: 파일 전송 ---*/

    // 파일 열기
    FILE *fp = fopen("test.mp4", "rb");
    int file_size;
    char data[1460];
    int file_len;
    int cnt = 0;

    // 1) 파일 사이즈 저장 및 전송: 그래야 전송 후에 제대로 전송이 되었는지 확인 가능
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    send(clnt_sock, (char *)&file_size, sizeof(int), 0);
    fseek(fp, 0, SEEK_SET);

    // 2) 파일 전송 (1460바이트씩)
    while (1)
    {
      file_len = fread(data, sizeof(char), 1460, fp); //  data버퍼에 저장(1바이트씩 총 1460개의 항목을 읽음 (파일 포인터의 위치는 읽은 바이트 만큼 증가)), 성공한 전체 항목 수 만큼 리턴 (총 바이트 수 리턴)
      send(clnt_sock, data, file_len, 0);

      cnt += 1;
      printf("transmit %4d bytes(:#%d) to client.\r", file_len, cnt);

      if (feof(fp))
        break;
    }

    printf("\n\ndone: Total size of file is %d bytes.\n", file_size);

    fclose(fp);
    /*--- // 파일 전송 완료 ---*/
  }
  /* n라는 답변이 오면 종료 */
  else
    printf("\n\ntermination.\n");

  /* 소켓 닫기 */
  close(clnt_sock); // 클라이언트 소켓이 먼저 닫혀야 함
  close(serv_sock); // 서버 소켓 닫기

  return 0;
}
