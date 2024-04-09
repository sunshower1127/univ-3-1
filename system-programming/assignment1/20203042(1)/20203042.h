#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse_str_in_bytes(char *input, int bytes);
void dump_memory(const void *mem, size_t len);
void set_memory(void *mem, char *data, size_t len);

// 바이너리 문자열을 해당 바이트 개수만큼 바이트 단위로 거꾸로 변환시킨다.
// 예) reverse_str_in_bytes("0000111111110000", 2) -> 1111000000001111
// 리틀엔디안 구조로 메모리에 저장되는 데이터를 고려하기 위해 만들었다.
void reverse_str_in_bytes(char *input, int bytes)
{
    char *output = (char *)malloc(bytes * 8);

    for (int i = 0; i < bytes; i++)
        memcpy(output + i * 8, input + (bytes - i - 1) * 8, 8);

    memcpy(input, output, bytes * 8);

    free(output);
}

// 테스트용 메모리 덤프 함수
void dump_memory(const void *mem, size_t len)
{
    const char *buffer = mem;
    size_t i = 0;
    for (i = 0; i < len; i++)
    {
        if (i > 0 && i % 8 == 0)
            printf("\n");
        printf("%02x ", buffer[i] & 0xff);
    }
    if (i > 1 && i % 8 != 1)
        puts("");
}

// 메모리에 바이너리 문자열 데이터를 1비트씩 직접 넣는 함수
void set_memory(void *mem, char *data, size_t len)
{
    memset(mem, 0, len / 8);
    char *buffer = mem;
    for (size_t i = 0; i < len; i++)
    {
        if (data[i] == '1')
            buffer[i / 8] |= 1 << (7 - i % 8);
    }
}