#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse_str(char *input, int bytes)
{
    char *output = (char *)malloc(bytes * 8);

    for (int i = 0; i < bytes; i++)
        memcpy(output + i * 8, input + (bytes - i - 1) * 8, 8);

    memcpy(input, output, bytes * 8);

    free(output);
}