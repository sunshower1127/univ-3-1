#include "mystdio.h"
#include <assert.h>

void test_fopen_fclose()
{
    FILE *fp = fopen("test.txt", "w");
    assert(fp != NULL);
    assert(fp->fd > 2);
    assert(fp->mode == 'w');
    assert(fp->buffer != NULL);
    assert(fp->buffer_pos == 0);
    assert(fp->eof == 0);
    assert(__open_files_count > 0);
    assert(__open_files[__open_files_count - 1] == fp);

    int result = fclose(fp);
}

void test_fread_fwrite()
{
    FILE *fp = fopen("test.txt", "w+");
    assert(fp != NULL);

    char write_buf[BUFSIZE] = "Hello, world!";
    int write_count = fwrite(write_buf, 1, strlen(write_buf), fp);
    assert(write_count == strlen(write_buf));

    fseek(fp, 0, SEEK_SET);

    char read_buf[BUFSIZE] = {0};
    int read_count = fread(read_buf, 1, BUFSIZE, fp);
    assert(read_count == write_count);
    assert(strcmp(read_buf, write_buf) == 0);

    fclose(fp);
}

void test_fflush()
{
    FILE *fp = fopen("test.txt", "w");
    assert(fp != NULL);

    char buf[BUFSIZE] = "Hello, world!";
    fwrite(buf, 1, strlen(buf), fp);
    assert(fp->buffer_pos == strlen(buf));

    fflush(fp);
    assert(fp->buffer_pos == 0);

    fclose(fp);
}

void test_fseek_feof()
{
    FILE *fp = fopen("test.txt", "w+");
    assert(fp != NULL);

    char buf[BUFSIZE] = "Hello, world!";
    fwrite(buf, 1, strlen(buf), fp);

    fseek(fp, 0, SEEK_SET);
    assert(fp->buffer_pos == 0);

    fseek(fp, 0, SEEK_END);
    assert(feof(fp) == 0);

    char c;
    fread(&c, 1, 1, fp);
    assert(feof(fp) == EOF);

    fclose(fp);
}

int main()
{
    test_fopen_fclose();
    test_fread_fwrite();
    test_fflush();
    test_fseek_feof();

    return 0;
}