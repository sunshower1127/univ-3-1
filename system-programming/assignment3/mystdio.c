#include "mystdio.h"

int main()
{
    FILE *fp = fopen("test.txt", "w");
    const char *str = "Hello, world!";
    fwrite(str, 1, strlen(str), fp);
    fclose(fp);
    return 0;
}