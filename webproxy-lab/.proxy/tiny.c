/* $begin tinymain */ // tinymain 블록 시작 (교재 표시용) 
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "csapp.h" // csapp 유틸리티 함수/타입 포함

void doit(int fd); // 연결 처리 함수 선언
void read_requesthdrs(rio_t *rp); // 요청 헤더 읽기 함수 선언
int parse_uri(char *uri, char *filename, char *cgiargs); // URI 파싱 선언
void serve_static(int fd, char *filename, int filesize); // 정적 파일 제공 선언
void get_filetype(char *filename, char *filetype); // 파일 타입 결정 선언
void serve_dynamic(int fd, char *filename, char *cgiargs); // 동적 컨텐츠 제공 선언
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg); // 클라이언트 에러 응답 선언
     
int main(int argc, char **argv) // 프로그램 진입점
{
  int listenfd, connfd; // 리스닝/연결 소켓
  char hostname[MAXLINE], port[MAXLINE]; // 클라이언트 호스트/포트 버퍼
  socklen_t clientlen; // 클라이언트 주소 길이
  struct sockaddr_storage clientaddr; // 클라이언트 주소 저장 구조체

  /* Check command line args */ // 명령행 인자 검사
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]); // 사용법 출력
    exit(1); // 잘못된 인자면 종료
  }

  listenfd = Open_listenfd(argv[1]); // 포트로 리스닝 소켓 오픈
  signal(SIGCHLD, sigchld_handler); // 좀비 프로세스 방지용 시그널 핸들러 등록
  while (1) // 반복적으로 연결 수락                          
  {
    clientlen = sizeof(clientaddr); // 주소 길이 초기화
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen); // 연결 수락
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 
                0); // 클라이언트 이름/포트 얻기
    printf("Accepted connection from (%s, %s)\n", hostname, port); // 로그 출력
    doit(connfd);  // 요청 처리 함수 호출
    Close(connfd); // 연결 소켓 닫기
  }
 
  return 0; // 정상 종료
}

void doit(int fd){ // 단일 연결 처리 함수
  int is_static; // 정적 여부 플래그
  struct stat sbuf; // 파일 정보 저장 구조체
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE]; // 요청 버퍼들
  char filename[MAXLINE], cgiargs[MAXLINE]; // 파일명 및 CGI 인자 버퍼
  rio_t rio; // robust I/O 구조체

  /* Read request line and headers */ // 요청 라인 및 헤더 읽기
  Rio_readinitb(&rio, fd); // rio 초기화
  Rio_readlineb(&rio, buf, MAXLINE); // 요청 라인 읽기
  printf("Request headers:\n"); // 로그 헤더
  printf("%s", buf); // 요청 라인 출력
  sscanf(buf, "%s %s %s", method, uri, version); // 요청 라인 파싱
  if (strcasecmp(method, "GET")) // GET 이외의 메소드 체크
  {
    clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method"); // 구현되지 않은 메소드 응답
    return; // 처리 종료

  }
  read_requesthdrs(&rio); // 남은 요청 헤더 읽기
  /* Parse URI from GET request */ // URI 파싱
  is_static = parse_uri(uri, filename, cgiargs); // 정적/동적 판별 및 파일명/인자 설정
  if (stat(filename, &sbuf) < 0) // 파일 존재 여부 검사
  {
    clienterror(fd, filename, "404", "Not found",
                "Tiny couldn't find this file"); // 파일 없음 응답
    return;       
      
}
  if (is_static)
  { /* Serve static content */ // 정적 컨텐츠 제공 분기
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) // 읽기 권한/정규 파일 검사
    {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't read the file"); // 읽기 불가 응답
      return;
    }
    serve_static(fd, filename, sbuf.st_size); // 정적 파일 전송
  }   
  else
  { /* Serve dynamic content */ // 동적 컨텐츠 제공 분기
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) // 실행 권한/정규 파일 검사
    //sbuf.st_mode: 이 필드에는 해당 파일의 유형(type) 정보와 접근 권한(permission) 정보가 비트(bit)들의 조합으로 압축되어 저장되어 있습니다.
    // S_ISREG와 S_IRUSR은 바로 이 st_mode 값에서 원하는 정보를 추출해내는 도구.
    {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't run the CGI program"); // 실행 불가 응답
      return;
    }
    serve_dynamic(fd, filename, cgiargs); // CGI 프로그램 실행
  }
}

void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg){ // 클라이언트 오류 응답 생성
  char buf[MAXLINE], body[MAXBUF]; // 헤더/본문 버퍼
  int blen = 0, hlen = 0; // 본문/헤더 길이 카운터

  /* Build the HTTP response body safely */ // 본문 안전하게 작성
  blen += snprintf(body+blen, MAXBUF-blen, "<html><title>Tiny Error</title>"); // 제목 추가
  blen += snprintf(body+blen, MAXBUF-blen, "<body bgcolor=\"ffffff\">\r\n"); // 바디 시작 태그
  blen += snprintf(body+blen, MAXBUF-blen, "<h1>%s: %s</h1>\r\n", errnum, shortmsg); // 에러 헤더 추가
  blen += snprintf(body+blen, MAXBUF-blen, "<p>%s: %s\r\n", longmsg, cause); // 에러 설명 추가
  blen += snprintf(body+blen, MAXBUF-blen, "<hr><em>The Tiny Web server</em>\r\n"); // 푸터 추가

  /* Print the HTTP response headers safely */ // 헤더 안전하게 작성 및 전송
  hlen += snprintf(buf+hlen, MAXLINE-hlen, "HTTP/1.0 %s %s\r\n", errnum, shortmsg); // 상태 라인 작성
  Rio_writen(fd, buf, strlen(buf)); // 상태 라인 전송
  hlen = 0; // 헤더 버퍼 재사용을 위해 초기화
  hlen += snprintf(buf+hlen, MAXLINE-hlen, "Content-type: text/html\r\n"); // Content-type 작성
  Rio_writen(fd, buf, strlen(buf)); // 전송
  hlen = 0; // 다시 초기화
  hlen += snprintf(buf+hlen, MAXLINE-hlen, "Content-length: %d\r\n\r\n", blen); // Content-length 작성
  Rio_writen(fd, buf, strlen(buf)); // 전송
  Rio_writen(fd, body, blen); // 본문 전송
}
void read_requesthdrs(rio_t *rp){ // 요청 헤더를 끝까지 읽는 함수
  char buf[MAXLINE]; // 라인 버퍼

  Rio_readlineb(rp, buf, MAXLINE); // 첫 헤더 라인 읽기
  while (strcmp(buf, "\r\n")) // 빈 라인(헤더 끝) 전까지 반복
  {
    Rio_readlineb(rp, buf, MAXLINE); // 다음 라인 읽기
    printf("%s", buf); // 헤더 라인 로그 출력
  }
  return; // 반환
}
int parse_uri(char *uri, char *filename, char *cgiargs){ // URI 파싱 함수
  char *ptr; // 임시 포인터

  if (!strstr(uri, "cgi-bin")) // cgi-bin 포함 여부 확인
  { /* Static content */ // 정적 컨텐츠인 경우
    strcpy(cgiargs, ""); // CGI 인자 비움
    strcpy(filename, "."); // 현재 디렉터리로 시작
    strcat(filename, uri); // URI를 파일 경로로 결합
    if (uri[strlen(uri) - 1] == '/') // 경로가 '/'로 끝나면
      strcat(filename, "home.html"); // 기본 파일 추가
    return 1; // 정적 표시
  }
  else
  { /* Dynamic content */ // 동적 컨텐츠인 경우
    ptr = index(uri, '?'); // 쿼리 시작 포인터 찾기
    if (ptr)
    {
      strcpy(cgiargs, ptr + 1); // ? 이후를 CGI 인자로 복사
      *ptr = '\0'; // URI에서 ? 제거
    }
    else
      strcpy(cgiargs, ""); // 인자 없으면 빈 문자열
    strcpy(filename, "."); // 파일 경로 구성
    strcat(filename, uri); // URI를 파일 경로로 결합
    return 0; // 동적 표시
  }
}
/* code/netp/tiny/tiny.c */
void serve_static(int fd, char *filename, int filesize)
{
    // char *srcp; // mmap에서 사용하던 포인터이므로 삭제 또는 주석 처리
    int srcfd;
    char *buf; // 새로 추가된 버퍼 포인터
    char filetype[MAXLINE], header_buf[MAXBUF];

    /* Send response headers to client */
    get_filetype(filename, filetype);
    sprintf(header_buf, "HTTP/1.0 200 OK\r\n");
    sprintf(header_buf, "%sServer: Tiny Web Server\r\n", header_buf);
    sprintf(header_buf, "%sConnection: close\r\n", header_buf);
    sprintf(header_buf, "%sContent-length: %d\r\n", header_buf, filesize);
    sprintf(header_buf, "%sContent-type: %s\r\n\r\n", header_buf, filetype);
    Rio_writen(fd, header_buf, strlen(header_buf));
    printf("Response headers:\n");
    printf("%s", header_buf);

    /* --- 수정된 본문 전송 로직 --- */
    srcfd = Open(filename, O_RDONLY, 0);
    buf = (char *)Malloc(filesize);
    Rio_readn(srcfd, buf, filesize);
    Close(srcfd);
    Rio_writen(fd, buf, filesize);
    Free(buf);
    /* --- 수정 끝 --- */
}    
void get_filetype(char *filename, char *filetype){ // 파일 확장자로 Content-type 결정
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html"); // html
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif"); // gif
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png"); // png
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg"); // jpg/jpeg
  else if(filetype, ".mpg")
    strcpy(filetype, "video/mpg");
  else
    strcpy(filetype, "text/plain"); // 기본은 텍스트
}
void sigchld_handler(int sig) {
    int old_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = old_errno;
}
void serve_dynamic(int fd, char *filename, char *cgiargs){ // CGI 실행 함수
  char buf[MAXLINE], *emptylist[] = {NULL};  // 헤더 버퍼 및 빈 인자 리스트
  /* Return first part of HTTP response */ // 상태 라인 전송
  sprintf(buf, "HTTP/1.0 200 OK\r\n"); // 상태 라인 작성
  Rio_writen(fd, buf, strlen(buf)); // 전송
  sprintf(buf, "Server: Tiny Web Server\r\n"); // 서버 헤더 작성
  Rio_writen(fd, buf, strlen(buf)); // 전송
  if (Fork() == 0)
  { /* Child process */ // 자식 프로세스에서 CGI 실행
    /* Real server would set all CGI vars here */
    setenv("QUERY_STRING", cgiargs, 1); // QUERY_STRING 환경변수 설정
    Dup2(fd, STDOUT_FILENO);         /* Redirect stdout to client */ // stdout을 소켓으로 리디렉트
    Execve(filename, emptylist, environ); /* Run CGI program */ // CGI 실행
  }
}
/* $end tinymain */ // tinymain 블록 끝
