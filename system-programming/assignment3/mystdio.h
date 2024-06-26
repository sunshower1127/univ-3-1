#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// 파일 구조체들을 저장할 배열의 최대 크기
#define __MAX_FILES (10000)

#define BUFSIZE (1024)
#define EOF (-1)

// stdin, stdout, stderr 구현
#define stdin __getstd(0)
#define stdout __getstd(1)
#define stderr __getstd(2)

typedef struct myFile
{
    int fd;
    int mode;      // 'r', 'w', 'a'
    int plus_mode; // 'r+', 'w+', 'a+'
    char lastop;   // 'r' or 'w'
    int eof;       // 0: not EOF, -1: EOF
    char *buffer;
    int buffer_pos;
    int buffer_size;
} FILE;

FILE __stdin = {.fd = 0, .mode = 'r'};
FILE __stdout = {.fd = 1, .mode = 'w'};
FILE __stderr = {.fd = 2, .mode = 'w'};

// 열린 파일들을 저장할 배열 -> 프로그램 exit시 fclose를 호출하기 위함
FILE *__open_files[__MAX_FILES];
int __open_files_count = 0;
int __atexit_count = 0;

FILE *__getstd(int fd);
void __atexit();
FILE *fopen(const char *pathname, const char *mode);
int fread(void *ptr, int size, int nmemb, FILE *stream);
int fwrite(const void *ptr, int size, int nmemb, FILE *stream);
int fflush(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
int feof(FILE *stream);
int fclose(FILE *stream);

// stdin, stdout, stderr 구현
FILE *__getstd(int fd)
{
    if (__atexit_count == 0)
    {
        // 프로그램 종료시 __atexit 함수 호출
        atexit(__atexit);
        __atexit_count++;
    }

    switch (fd)
    {
    case 0:
        // 처음 호출시 buffer 동적 할당 & FILE* 배열에 저장
        if (__stdin.buffer == NULL)
        {
            __stdin.buffer = (char *)malloc(BUFSIZE);
            if (__open_files_count < __MAX_FILES)
                __open_files[__open_files_count++] = &__stdin;
        }
        return &__stdin;
    case 1:
        if (__stdout.buffer == NULL)
        {
            __stdout.buffer = (char *)malloc(BUFSIZE);
            if (__open_files_count < __MAX_FILES)
                __open_files[__open_files_count++] = &__stdout;
        }
        return &__stdout;
    case 2:
        if (__stderr.buffer == NULL)
        {
            __stderr.buffer = (char *)malloc(BUFSIZE);
            if (__open_files_count < __MAX_FILES)
                __open_files[__open_files_count++] = &__stderr;
        }
        return &__stderr;
    default:
        return NULL;
    }
}

// 모든 FILE fclose
void __atexit()
{
    for (int i = 0; i < __MAX_FILES; i++)
    {
        if (__open_files[i] != NULL)
            fclose(__open_files[i]);
    }
}

// 정상 return -> File*, 에러 return -> NULL
FILE *fopen(const char *pathname, const char *mode)
{
    FILE *fp = (FILE *)malloc(sizeof(FILE));

    fp->plus_mode = (strlen(mode) == 2 && mode[1] == '+') ? 1 : 0;

    // fopen의 mode와 open의 flags를 맵핑
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

    // 파일 열기
    fp->fd = open(pathname, flags, 0666);
    if (fp->fd < 0)
    {
        free(fp);
        return NULL;
    }

    fp->mode = mode[0];
    fp->lastop = 0;
    fp->eof = 0;
    fp->buffer = (char *)malloc(BUFSIZE);
    fp->buffer_pos = 0;
    fp->buffer_size = 0;

    // 프로그램 exit될때 fclose를 호출하기 위해서 FILE*를 저장
    if (__open_files_count < __MAX_FILES)
        __open_files[__open_files_count++] = fp;

    // atexit에 등록
    if (__atexit_count == 0)
    {
        atexit(__atexit);
        __atexit_count++;
    }

    return fp;
}

// 정상 return -> 읽은 요소의 개수, 에러 return -> 0
int fread(void *ptr, int size, int nmemb, FILE *stream)
{
    // "w", "a" 모드에선 읽기 불가
    if (stream->mode != 'r' && !stream->plus_mode)
        return 0;

    // 쓰기 버퍼 비우기
    if (stream->lastop == 'w')
        fflush(stream);

    stream->lastop = 'r';

    int total = size * nmemb;
    int read_size = 0;
    int read_total = 0;

    // 버퍼에서 읽어오고, 버퍼에 없다면 파일에서 버퍼로 가져와서 읽어옴.
    while (read_total < total)
    {
        if (stream->buffer_pos == 0)
        {
            stream->buffer_size = read(stream->fd, stream->buffer, BUFSIZE);
            if (stream->buffer_size == 0)
            {
                stream->eof = EOF;
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

// 정상 return -> 쓴 요소의 개수, 에러 return -> 0
int fwrite(const void *ptr, int size, int nmemb, FILE *stream)
{
    // "r" 모드에선 쓰기 불가
    if (stream->mode == 'r' && !stream->plus_mode)
        return 0;

    // "a+" 모드에서 이전에 read를 했었다면 write를 할땐 파일 끝으로 이동시킴.
    if (stream->mode == 'a' && stream->plus_mode && stream->lastop == 'r')
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

    // 버퍼에 쓰고, 버퍼가 가득차면 파일로 쓰기
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

// 정상 return -> 0, 에러 return -> EOF
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

// 정상 return -> 0, 에러 return -> -1
int fseek(FILE *stream, long offset, int whence)
{
    if (stream->mode == 'a' && !stream->plus_mode)
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

// EOF이면 -1, 아니면 0 리턴
int feof(FILE *stream)
{
    if (stream->eof == EOF)
        return -1;
    else
        return 0;
}

// 정상 return -> 0, 에러 return -> EOF
int fclose(FILE *stream)
{
    if (stream == NULL)
        return EOF;
    if (stream->lastop == 'w')
        fflush(stream);

    if (close(stream->fd) == -1)
        return EOF;
    free(stream->buffer);

    // stdin, stdout, stderr는 free하지 않음 -> 전역변수라서
    if (stream == &__stdin || stream == &__stdout || stream == &__stderr)
        return 0;

    // 사용자가 프로그램이 exit하기 전에 fclose를 호출할 경우 대비
    for (int i = 0; i < __MAX_FILES; i++)
    {
        if (__open_files[i] == stream)
        {
            __open_files[i] = NULL;
            break;
        }
    }
    free(stream);

    return 0;
}
