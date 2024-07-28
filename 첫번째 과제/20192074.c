// 헤더 파일
#include <20192074.h>

// 8개의 '0'과 '1'로 구성된 문자열을 바이트로 변환
unsigned char binary_string_to_byte(const char *binaryString) {
  unsigned char result = 0; // 반환할 값을 선언하고 초기에 0을 대입.
  for (int i = 0; i < 8; ++i) // 8개의 ‘0’과 ‘1’이 있으므로 총 8번의 반복문이 실행.
  { 
    result <<= 1; // 왼쪽으로 1비트 시프트, 이후 그 결과를 다시 result에 할당.
    if (binaryString[i] == '1') // 만약에 현재 비트가 1인 경우에.
    {
      result |= 1; // result 변수에 대해서 OR 연산, 이후 그 결과를 다시 result에 할당.
    }
  }
  return result; // result 반환
}

// 32개의 '0'과 ‘1'을 문자열로 받아와서 이를 unsigned int 타입으로 반환
unsigned int binary_string_to_4byte(const char *binaryString) { // 매개변수로 문자열을 받음.
  unsigned int result = 0; // 반환할 값을 선언하고 초기에 0을 대입.
  for (int i = 0; i < 32; ++i) // 32개의 ‘0’과 ‘1’이 있으므로 총 32번의 반복문이 실행.
  {
    result <<= 1; // 왼쪽으로 1비트 시프트, 이후 그 결과를 다시 result에 할당.
    if (binaryString[i] == '1') // 만약에 현재 비트가 1인 경우에.
    {
      result |= 1; // result 변수에 대해서 OR 연산, 이후 그 결과를 다시 result에 할당.
    }
  }
  return result; // result 반환
}

// 메모리 주소와 사이즈를 인자로 받아와서 메모리에 적재되어 있는 값을 signed char 형태로 반환
void dump_mem_signed_char(const unsigned char *mem, size_t len) { // 매개변수로 메모리 주소와 사이즈를 받음.
  printf("1.signed char : ");
  for (int i = len-1; i >= 0; i--) // 엔디안을 고려해서 내림차순으로 반복문을 실행. 명세 결과와 동일하게 하기 위함.
  {
    printf("%d ", (signed char)mem[i]); // 메모리에 할당된 값을 출력.
    if ((i + 1) % 16 == 0) // i+1이 16번째가 될때마다 줄내림을 실행.
    {
      printf("\n");
    }
  }
  printf("\n"); // 줄내림.
}

// 메모리 주소와 사이즈를 인자로 받아와서 메모리에 적재되어 있는 값을 ASCII codes형태로 반환
void dump_mem_ASCII_codes(const unsigned char *mem, size_t len) { // 매개변수로 메모리 주소와 사이즈를 받음.
  printf("2.ASCII codes : ");
  for (int i = len-1; i >= 0; i--) // 엔디안을 고려해서 내림차순으로 반복문을 실행. 명세 결과와 동일하게 하기 위함.
  {
    if (mem[i] >= 32 && mem[i] <= 126) // 대응되는 값이 있다면 되는 값을 출력.
    { 
      printf("%c", mem[i]);
    }
    else // 대응되는 값이 없다면 명세에 따라 . 을 출력.
    {
      printf("."); 
    }
    if ((i + 1) % 16 == 0) // i+1이 16번째가 될때마다 줄내림을 실행.
    {
      printf("\n");
    }
  }
  printf("\n"); // 줄내림.
}

// 메모리 주소와 사이즈를 인자로 받아와서 메모리에 적재되어 있는 값을 unsigned char 형태로 반환
void dump_mem_unsigned_char(const unsigned char *mem, size_t len) { // 매개변수로 메모리 주소와 사이즈를 받음.
  printf("3.unsigned char : ");
  for (int i = len-1; i >= 0; i--) // 엔디안을 고려해서 내림차순으로 반복문을 실행. 명세 결과와 동일하게 하기 위함.
  {
    printf("%dd ", (unsigned char)mem[i]); // 메모리에 할당된 값을 출력.
    if ((i + 1) % 16 == 0) // i+1이 16번째가 될때마다 줄내림을 실행.
    {
      printf("\n");
    }
  }
  printf("\n"); // 줄내림.
}

// 메모리 주소와 사이즈를 인자로 받아와서 메모리에 적재되어 있는 값을 unsigned int 형태로 반환.
void dump_mem_signed_int(const unsigned int *mem, size_t len) {
  printf("4.signed Int : ");
  for (int i = len-1; i >= 0; i--) // 엔디안을 고려해서 내림차순으로 반복문을 실행. 명세 결과와 동일하게 하기 위함.
  {
    printf("%d ", (signed int)mem[i]); // 메모리에 할당된 값을 출력.
    if ((i + 1) % 16 == 0) // i+1이 16번째가 될때마다 줄내림을 실행.
    {
      printf("\n");
    }
  }
  printf("\n"); // 줄내림.
}

// 메모리 주소와 사이즈를 인자로 받아와서 메모리에 적재되어 있는 값을 unsigned int 형태로 반환.
void dump_mem_unsigned_int(const unsigned int *mem, size_t len) {
  printf("5.unsigned Int : ");
  for (int i = len-1; i >= 0; i--) // 엔디안을 고려해서 내림차순으로 반복문을 실행. 명세 결과와 동일하게 하기 위함.
  {
    printf("%u ", (unsigned int)mem[i]); // 메모리에 할당된 값을 출력.
    if ((i + 1) % 16 == 0) // i+1이 16번째가 될때마다 줄내림을 실행.
    {
      printf("\n");
    }
  }
  printf("\n"); // 줄내림.
}

int main() {
  FILE *file = fopen("input", "rb"); // input file을 읽어오는데 바이너리 형태로 읽음.
  if (file == NULL) // 부른 파일이 없다면.
  { 
    printf("파일을 열 수 없습니다.\n"); // 해당 메시지 출력 이후.
    exit(1); // 프로그램 종료.
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file); // 파일의 크기 측정
  rewind(file); // 파일의 커서를 맨 앞으로 이동.

  // 메모리 동적할당
  unsigned char *byte_1_Buffer = malloc(file_size / 4); // 1바이트씩 자른 값을 저장하는 메모리
  unsigned int *byte_4_Buffer = malloc(file_size / 32); // 4바이트씩 자른 값을 저장하는 메모리
  if (byte_1_Buffer == NULL || byte_4_Buffer == NULL) // 메모리가 할당이 되지 않았다면
  {
    printf("메모리 할당 중 에러가 났습니다.\n"); // 해당 메시지 출력 이후.
    fclose(file); // 파일 닫고.
    exit(2); // 프로그램 종료.
  }

  //
  char byte_1_bit[9]; // 8개의 비트와 1개의 널문자를 넣기 위해서 9개를 문자를 담을 수 있는 배열 선언
  int byte_1_index = 0; // 1바이트씩 자른 값을 저장하는 인덱스
  while (fread(byte_1_bit, 1, 8, file) == 8) { // 파일로부터 1바이트씩 읽어와서 8개의 문자를 byte_1_bit 저장하고 만약 그 크기가 8이 된다면.
    byte_1_bit[8] = '\0'; // 마지막에 널문자 처리
    byte_1_Buffer[byte_1_index++] = binary_string_to_byte(byte_1_bit); // 0과 1로만 이루어진 문자열을 binary_string_to_byte 함수에 넣어 해당 문자열을 unsigned char 형태로 반환. 그리고 반환한 값을 메모리에 적재
  }

  dump_mem_signed_char(byte_1_Buffer, byte_1_index); // signed_char 출력
  dump_mem_ASCII_codes(byte_1_Buffer, byte_1_index); // ASCII_codes 출력
  dump_mem_unsigned_char(byte_1_Buffer, byte_1_index); // unsigned_char 출력

  rewind(file); // fread를 통해서 커서가 이미 이동을 했을 것이므로 파일의 커서를 맨 앞으로 이동.

  char byte_4_bit[33]; // 32개의 비트와 1개의 널문자를 넣기 위해서 33개를 문자를 담을 수 있는 배열 선언
  int byte_4_index = 0; // 4바이트씩 자른 값을 저장하는 인덱스
  while (fread(byte_4_bit, 1, 32, file) == 32) { // 파일로부터 1바이트씩 읽어와서 32개의 문자를 byte_1_bit 저장하고 만약 그 크기가 32이 된다면.
    byte_4_bit[32] = '\0'; // 마지막에 널문자 처리
    byte_4_Buffer[byte_4_index++] = binary_string_to_4byte(byte_4_bit); // 0과 1로만 이루어진 문자열을 binary_string_to_4byte 함수에 넣어 해당 문자열을 unsigned int 형태로 반환. 그리고 반환한 값을 메모리에 적재
  }

  dump_mem_signed_int(byte_4_Buffer, byte_4_index); // singed_int 출력
  dump_mem_unsigned_int(byte_4_Buffer, byte_4_index); // unsiged_int 출력

  fclose(file); // 파일 닫고
  free(byte_1_Buffer); // 동적 할당한 메모리 해제
  free(byte_4_Buffer); // 동적 할당한 메모리 해제

  return 0; // 프로그램 종료
}
