#include <assert.h>
#include "mystdio.h"

void test_fopen_fclose()
{
    FILE *fp = fopen("test.txt", "w+");
    assert(fp != NULL);
    assert(fclose(fp) == 0);
}

void test_fwrite_fread()
{
    FILE *fp = fopen("test.txt", "w+");
    assert(fp != NULL);

    char write_buf[] = "Hello, World!";
    assert(fwrite(write_buf, sizeof(char), sizeof(write_buf), fp) == sizeof(write_buf));

    assert(fseek(fp, 0, SEEK_SET) == 0);

    char read_buf[sizeof(write_buf)];
    assert(fread(read_buf, sizeof(char), sizeof(read_buf), fp) == sizeof(read_buf));

    assert(strcmp(write_buf, read_buf) == 0);

    assert(fclose(fp) == 0);
}

void test_fflush()
{
    FILE *fp = fopen("test.txt", "w");
    assert(fp != NULL);

    char buf[] = "Hello, World!";
    assert(fwrite(buf, sizeof(char), sizeof(buf), fp) == sizeof(buf));

    assert(fflush(fp) == 0);

    assert(fclose(fp) == 0);
}

void test_feof()
{
    FILE *fp = fopen("test.txt", "r");
    assert(fp != NULL);

    while (!feof(fp))
    {
        char buf[BUFSIZE];
        fread(buf, sizeof(char), BUFSIZE, fp);
    }

    assert(feof(fp) == TRUE);

    assert(fclose(fp) == 0);
}

int main()
{
    test_fopen_fclose();
    test_fwrite_fread();
    test_fflush();
    test_feof();
    printf("All tests passed.\n");
    return 0;
}