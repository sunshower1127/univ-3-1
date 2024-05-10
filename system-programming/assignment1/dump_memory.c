#include <stdio.h>

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

// int main()
// {
//     char c = 0x80;
//     int i = c;
//     dump_memory(&i, sizeof(i));
//     return 0;
// }