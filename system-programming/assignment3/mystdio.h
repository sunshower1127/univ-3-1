#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE (1024)
#define EOF (-1)
#define stdin (0)
#define stdout (1)
#define stderr (2)

#define FALSE (0)
#define TRUE (1)

typedef struct myFile
{
    int fd;
    int mode;
    int plus_mode;
    char lastop; // 'r' or 'w'
    int eof;
    char *buffer;
    int buffer_pos;
    int buffer_size;
} FILE;

FILE *fopen(const char *pathname, const char *mode);
int fread(void *ptr, int size, int nmemb, FILE *stream);
int fwrite(const void *ptr, int size, int nmemb, FILE *stream);
int fflush(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
int feof(FILE *stream);
int fclose(FILE *stream);

FILE *fopen(const char *pathname, const char *mode)
{
    FILE *fp = (FILE *)malloc(sizeof(FILE));

    fp->plus_mode = (strlen(mode) == 2 && mode[1] == '+') ? TRUE : FALSE;
    int flags;

    switch (mode[0])
    {
    case 'r':
        flags = fp->plus_mode ? O_RDWR : O_RDONLY;
        break;
    case 'w':
        flags = O_CREAT | O_TRUNC | (fp->plus_mode ? O_RDWR : O_WRONLY);
        break;
    case 'a':
        flags = O_CREAT | O_APPEND | (fp->plus_mode ? O_RDWR : O_WRONLY);
        break;
    default:
        free(fp);
        return NULL;
    }

    fp->fd = open(pathname, flags, 0666);
    if (fp->fd < 0)
    {
        free(fp);
        return NULL;
    }

    fp->mode = mode[0];
    fp->lastop = 0;
    fp->eof = FALSE;
    fp->buffer = (char *)malloc(BUFSIZE);
    fp->buffer_pos = 0;
    fp->buffer_size = 0;

    return fp;
}

int fread(void *ptr, int size, int nmemb, FILE *stream)
{
    if (stream->mode != 'r' && stream->plus_mode == FALSE)
        return 0;

    if (stream->lastop == 'w')
        fflush(stream);

    stream->lastop = 'r';

    int total = size * nmemb;
    int read_size = 0;
    int read_total = 0;

    while (read_total < total)
    {
        if (stream->buffer_pos == 0)
        {
            stream->buffer_size = read(stream->fd, stream->buffer, BUFSIZE);
            if (stream->buffer_size == 0)
            {
                stream->eof = TRUE;
                break;
            }
        }

        read_size = total - read_total;
        if (read_size > stream->buffer_size - stream->buffer_pos)
            read_size = stream->buffer_size - stream->buffer_pos;

        memcpy((char *)ptr + read_total, stream->buffer + stream->buffer_pos, read_size);
        stream->buffer_pos += read_size;
        read_total += read_size;

        if (stream->buffer_pos == stream->buffer_size)
            stream->buffer_pos = 0;
    }

    return read_total / size;
}

int fwrite(const void *ptr, int size, int nmemb, FILE *stream)
{
    // "r" 모드에선 쓰기 불가
    if (stream->mode == 'r' && stream->plus_mode == FALSE)
        return 0;

    // "a+" 모드에서 이전에 read를 했었다면 write를 할땐 파일 끝으로 이동시킴.
    if (stream->mode == 'a' && stream->plus_mode == TRUE && stream->lastop == 'r')
        fseek(stream, 0, SEEK_END);

    // 읽기 버퍼 비우기
    if (stream->lastop == 'r')
    {
        stream->buffer_pos = 0;
        stream->buffer_size = 0;
    }

    stream->lastop = 'w';

    int total = size * nmemb;
    int write_size = 0;
    int write_total = 0;

    while (write_total < total)
    {
        write_size = total - write_total;
        if (write_size > BUFSIZE - stream->buffer_pos)
            write_size = BUFSIZE - stream->buffer_pos;

        memcpy(stream->buffer + stream->buffer_pos, (char *)ptr + write_total, write_size);
        stream->buffer_pos += write_size;
        write_total += write_size;

        if (stream->buffer_pos == BUFSIZE)
        {
            write(stream->fd, stream->buffer, BUFSIZE);
            stream->buffer_pos = 0;
        }
    }

    return write_total / size;
}

int fflush(FILE *stream)
{
    // read buffer에선 안씀
    if (stream->lastop == 'r')
        return EOF;

    // write buffer에 남은 데이터가 있으면 write buffer 비우기
    if (stream->buffer_pos > 0)
    {
        write(stream->fd, stream->buffer, stream->buffer_pos);
        stream->buffer_pos = 0;
    }

    return 0;
}

int fseek(FILE *stream, long offset, int whence)
{
    if (stream->mode == 'a' && stream->plus_mode == FALSE)
        return 0;

    // 쓰기 버퍼 남아있으면 싹 없애주고
    if (stream->lastop == 'w')
        fflush(stream);

    // 읽기 버퍼도 비워주기
    else if (stream->lastop == 'r')
    {
        stream->buffer_pos = 0;
        stream->buffer_size = 0;
    }

    if (lseek(stream->fd, offset, whence) == -1)
        return -1;

    return 0;
}

int feof(FILE *stream)
{
    return stream->eof;
}

int fclose(FILE *stream)
{
    if (stream->lastop == 'w')
        fflush(stream);

    close(stream->fd);
    free(stream->buffer);
    free(stream);

    return 0;
}
