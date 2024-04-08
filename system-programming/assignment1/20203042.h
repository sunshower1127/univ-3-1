#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse_str(char *input, int bytes);
void dump_memory(const void *mem, size_t len);

void reverse_str(char *input, int bytes)
{
    char *output = (char *)malloc(bytes * 8);

    for (int i = 0; i < bytes; i++)
        memcpy(output + i * 8, input + (bytes - i - 1) * 8, 8);

    memcpy(input, output, bytes * 8);

    free(output);
}

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

void set_memory(void *mem, char *data, size_t len)
{
    char *buffer = mem;
    for (size_t i = 0; i < len; i++)
        buffer[i / 8] |= (data[i] == '1') << (7 - i % 8);
}