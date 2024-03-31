#include <stdio.h>

void dump_memory(const void *mem, size_t len)
{
    const char *buffer = mem;
    size_t i = 0;
    for(i = 0; i < len; i++)
    {
        if(i > 0 && i % 8 == 0) printf("\n");
        printf("%02x ", buffer[i] & 0xff);
    }
    if(i > 1 && i % 8 != 1) puts("");
}

int main()
{
    char arr[8] = {112, 12, -81, -86, -118, 69, -79, 40};
    dump_memory(arr, sizeof(arr));
    return 0;
}