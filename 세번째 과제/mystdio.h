#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024
#define EOF -1
#define stdin 1
#define stdout 2
#define stderr 3
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct myFile {
 int fd; // 파일 디스크립터
 int pos; // 현재 파일의 포지션 (파일 내에서 현재 읽기/쓰기 위치)
 int size; // 파일의 크기, 파일의 전체 크기를 바이트 단위로 나타냄.
 int mode; // 파일을 열었을 때 어떠한 모드인지를 저장함. (읽기전용, 쓰기전굥, 읽기쓰기 가능)
 int flag; // 파일의 읽기 쓰기 등의 플래그를 나타냄
 bool eof; // 파일의 끝 여부
 char *buffer; // 파일로부터 읽거나 파일에 쓸 데이터를 임시 저장하는 곳
} FILE;

FILE *fopen(const char *filename, const char *mode);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmenb, FILE *stream);
int fflush(FILE *stream);
int fseek(FILE *stream, int offset, int whence);
int feof(FILE *stream);
int fclose(FILE *stream);

FILE *fopen(const char *filename, const char *mode) {
    // FILE 구조체에 대한 포인터를 동적 할당합니다.
    FILE *stream = malloc(sizeof(FILE));

    // 파일 포인터의 버퍼 사이즈를 동적할당합니다.
    stream->buffer = (char *)malloc(BUFSIZE); 
    if (stream->buffer == NULL) // 버퍼 할당에 실패하면,
    { 
        free(stream); // 할당받았던 파일 포인터 메모리를 해제합니다.
        return NULL; // NULL을 반환합니다.
    }

    // 파일 모드에 따라 파일 포인터의 모드 설정
    int m, o; // 파일 모드와 옵션을 저장할 변수
    switch (*mode) // mode 문자열의 첫 번째 문자에 따라 분기
    {
      case 'r': // 읽기 모드
        m = O_RDONLY; // 읽기 전용
        o = 0; // 추가 옵션 없음
        stream->mode = m;
        stream->flag = m | o; // 파일 오픈 플래그 설정
        stream->pos = 0; // 현재 위치 초기화
        stream->eof = false; // EOF 플래그 초기화
        break;
      
      case 'w': // 쓰기 모드
        memset(stream->buffer, 0, BUFSIZE);
        m = O_WRONLY; // 쓰기 전용
        o = O_CREAT | O_TRUNC; // 파일이 없으면 생성, 파일이 있으면 내용 삭제
        stream->mode = m;
        stream->flag = m | o; // 파일 오픈 플래그 설정
        stream->pos = 0; // 현재 위치 초기화
        stream->eof = false; // EOF 플래그 초기화
        break;

      case 'a': // 추가 모드
        m = O_WRONLY; // 쓰기 전용
        o = O_CREAT | O_APPEND; // 파일이 없으면 생성, 파일에 내용 추가
        stream->mode = m;
        stream->flag = m | o; // 파일 오픈 플래그 설정
        break;

      default: // 지원하지 않는 모드인 경우
        free(stream); // 할당받았던 메모리 해제
        free(stream->buffer);
        return NULL; // NULL 반환
        break;
    }

    if (*(mode+1) == '+') // 모드 문자열의 두 번째 문자가 '+'인 경우
    {
      m = O_RDWR; // 읽기/쓰기 모드
      stream->mode = m; // 잘못된 코드: 이 부분은 stream->mode = m; 이어야 합니다.
    }
    
    // 파일을 열고 파일 디스크립터를 얻습니다.
    int fd = open(filename, stream->flag, DEFFILEMODE);
    if (fd < 0) // 파일 열기 실패
    {
      free(stream->buffer); // 버퍼 메모리 해제
      free(stream); // 파일 포인터 메모리 해제
      return NULL; // NULL 반환
    }
    else
    {
      stream->fd = fd; // 파일 디스크립터 저장
    }

    // 파일의 크기를 가져옵니다.
    struct stat statbuf;
    int size = 0;
    if (fstat(fd, &statbuf) == 0)
    {
      size = statbuf.st_size;
      stream->size = size; // 파일 크기 저장
    }

    // 'a' 모드인 경우 파일 포인터의 위치를 파일의 끝으로 설정
    if (*mode == 'a')
    {
      stream->pos = size;
      stream->eof = true;
    }
    else if (*mode == 'r' || *(mode+1) == '+') // 'r' 모드 또는 '+가 있는 모드'인 경우,
    {
      // 파일 내용을 버퍼에 읽어들입니다.
      int r = read(fd, stream->buffer, BUFSIZE);
      if (r < 0) // 읽기 실패
      {
        free(stream->buffer); // 버퍼 메모리 해제
        free(stream); // 파일 포인터 메모리 해제
        close(fd); // 파일 닫기
        return NULL; // NULL 반환
      }
      else
      {
        stream->size = r;
      }
    }

    return stream; // 파일 포인터 반환
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t total = size * nmemb; // 읽어야 할 전체 바이트 수 계산
    size_t r = 0; // 현재까지 읽은 바이트 수
    char *p = (char *)ptr; // char 형태로 포인터 형 변환하여 사용하기 쉽게 함

    // 스트림 또는 스트림의 버퍼가 NULL인 경우, 읽기 작업을 수행할 수 없으므로 0 반환
    if (stream == NULL || stream->buffer == NULL)
    {
        return 0;
    }

    // 파일의 끝에 도달했거나, 요청한 바이트 수가 0인 경우, 더 이상 읽을 수 없으므로 0 반환
    if (stream->eof || total == 0)
    {
        return 0;
    }

    // 요청한 바이트 수를 모두 읽거나 파일의 끝에 도달할 때까지 반복
    while (r < total && !stream->eof)
    {
        // 현재 버퍼에서 읽을 수 있는 남은 바이트 수
        size_t bytes_in_buffer = stream->size - stream->pos;

        // 읽어야 할 바이트 수
        size_t bytes_to_read = (total - r < bytes_in_buffer) ? total - r : bytes_in_buffer;

        // 버퍼에서 데이터를 읽어오고 ptr에 저장
        memcpy(p + r, stream->buffer + stream->pos, bytes_to_read);

        // 읽은 바이트 수 갱신
        r += bytes_to_read;

        // 현재 버퍼의 위치 갱신
        stream->pos += bytes_to_read;

        // 버퍼의 끝에 도달한 경우
        if (stream->pos >= stream->size)
        {
            // 파일로부터 새로운 데이터를 버퍼에 읽어옴
            int bytes_read = read(stream->fd, stream->buffer, BUFSIZE);

            // 읽기 실패 또는 파일의 끝에 도달한 경우
            if (bytes_read <= 0)
            {
                // 파일의 끝에 도달했음을 표시
                stream->eof = true;
                break;
            }

            // 읽은 데이터의 크기 갱신
            stream->size = bytes_read;

            // 버퍼의 위치 초기화
            stream->pos = 0;
        }
    }

    // 읽은 총 바이트 수를 요소의 크기로 나누어, 성공적으로 읽은 요소의 개수 반환
    return r / size;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if (stream == NULL || ptr == NULL) // 스트림 또는 전달된 포인터가 NULL인 경우, 쓰기 작업 불가
    {
        return 0;
    }

    size_t total = size * nmemb; // 전체 쓰여야 할 바이트 수 계산
    size_t w = 0; // 현재까지 쓴 바이트 수
    const char *p = (const char *)ptr; // const char 형태로 포인터 형 변환

    // 요청한 바이트 수를 모두 쓰거나 에러가 발생할 때까지 반복
    while (w < total)
    {
        size_t spaceInBuffer = BUFSIZE - stream->pos; // 버퍼 내 남은 공간 계산
        size_t bytesToWrite = (total - w < spaceInBuffer) ? total - w : spaceInBuffer; // 이번에 쓸 바이트 수 결정

        // 버퍼에 데이터 복사
        memcpy(stream->buffer + stream->pos, p + w, bytesToWrite);
        stream->pos += bytesToWrite; // 버퍼의 현재 위치 갱신
        w += bytesToWrite; // 쓴 바이트 수 갱신

        if (stream->pos == BUFSIZE) // 버퍼가 가득 찬 경우, 파일에 쓰기
        {
            if (write(stream->fd, stream->buffer, BUFSIZE) != BUFSIZE) // 쓰기 실패
            {
                // 에러 처리가 필요한 경우, 여기에 코드 추가
                break; // 쓰기 실패 시 반복 종료
            }
            stream->pos = 0; // 버퍼 위치 초기화
        }
    }

    // 쓰기 작업이 성공적으로 끝났으나, 버퍼에 남은 데이터가 있다면 파일에 쓰기
    if (stream->pos > 0)
    {
        if (write(stream->fd, stream->buffer, stream->pos) != stream->pos)
        {
            // 에러 처리가 필요한 경우, 여기에 코드 추가
        }
        stream->pos = 0;
    }

    // 쓴 총 바이트 수를 요소의 크기로 나누어, 성공적으로 쓴 요소의 개수 반환
    return w / size;
}

int fflush(FILE *stream)
{
  // stream이 NULL이거나 버퍼가 NULL인 경우에는 EOF 리턴 함.
  if (stream == NULL || stream -> buffer == NULL)
  {
    return EOF;
  }

  // 읽기 전용의 경우에는 버퍼를 비욾 필요가 없기 때문에 EOF를 리턴함.
  if (stream->mode == O_RDONLY)
  {
    return EOF;
  }

  // 위치가 0보다 크거나 같으며 모드가 쓰기 전용 또는 읽기 쓰기 전용인 경우에
  if (stream->pos >= 0 && (stream->mode == O_WRONLY || stream->mode == O_RDWR))
  {
    // 실제 파일에 버퍼의 내용을 쓴다. 쓰기에 성공한 바이트 값을 written에 저장
    ssize_t written = write(stream->fd, stream->buffer, stream->pos);

    // 위 작업이 실패를 하게 되면 EOf를 리턴한다.
    if (written < 0)
    {
      return EOF;
    }

    // 버퍼의 내용을 파일에 성공적으로 썻기 때문에 strema의 위치를 0으로 설정함.
    stream->pos = 0;
  }

  // 해당 작업이 정상으로 끝이 났기 때문에 0을 return함.
  return 0;
}

int fseek(FILE *stream, int offset, int whence)
{
  if (stream == NULL)
  {
    return -1;
  }

  switch (whence)
  {
    case SEEK_CUR: // 현재 위치 기준
      if (stream->pos + offset > stream->size || stream->pos + offset < 0)
      {
        return -1;
      }
      stream->pos += offset;
      break;

    case SEEK_SET: // 파일 시작 위치 기준
      if (offset > stream->size || offset < 0)
      {
        return -1;
      }
      stream->pos = offset;
      break;

    case SEEK_END:
      if (stream->size + offset > stream->size || stream->size + offset < 0)
      {
        return -1;
      }
      stream->pos = stream->size + offset;
      break;
    
    default:
      return -1;
      break;
  }

  stream->eof = (stream->pos == stream->size);

  return 0;
}

int feof(FILE *stream)
{
  // 현재 스트림의 eof가 true(맨 마지막에 위치)한 경우에 
  if (stream->eof) {
    return EOF; // -1을 리턴함.
  }
  return 0;
}

int fclose(FILE *stream)
{
  // 파일을 닫을 때 stream의 버퍼에 있는 내용을 전부 파일에 기록한다.
  fflush(stream);
  // 파일 스크립터를 닫는다. 만약 실패할 경우 버퍼와 stream에 동적할당된 메모리를 해제하고 -1을 리턴한다.
  if(close(stream->fd) < 0) {
    free(stream->buffer);
    free(stream);
    return -1;
  }
  // 버퍼에 동적할당한 메모리를 free 시킨다.
  free(stream->buffer);
  // 동적할당한 메모를 free 시킨다.
  free(stream);

  // 위 과정이 모두 성공하였으므로 0을 리턴한다.
  return 0;
}