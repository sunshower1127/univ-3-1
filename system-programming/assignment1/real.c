#include <stdio.h>

void dump_memory(const void *mem, size_t len)
{
    const unsigned char *buffer = mem;
    size_t i = 0;
    for (i = 0; i < len; i++)
    {

        for (int j = 0; j < 8; j++)
            printf("%d", (buffer[i] >> (7 - j)) & 1);

        printf(" ");
    }

    printf("\n");
}

int main()
{
    char arr[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    dump_memory(arr, sizeof(arr));
    return 0;
}