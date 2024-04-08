#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4

void set_memory(void *mem, char *data, size_t len)
{
    char *buffer = mem;
    for (size_t i = 0; i < len; i++)
        buffer[i / 8] |= (data[i] == '1') << (7 - i % 8);
}

int main()
{
    char *data = "00000000000000010000001000000100";
    char *buffer = (char *)malloc(SIZE);
    memset(buffer, 0, SIZE);
    set_memory(buffer, data, strlen(data));
    for (int i = 0; i < SIZE; i++)
        printf("%d", buffer[i]);
    printf("\n");
    free(buffer);
    return 0;
}