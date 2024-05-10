#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE (1024)
#define EOF (-1)
#define stdin (0)
#define stdout (1)
#define stderr (2)

#define NULL (0)
#define FALSE (0)
#define TRUE (1)


typedef struct myFile {
    int fd;
    int pos;
    int size;
    int mode;
    int flag;
    char lastop;
    int eof;
    char *buffer;
} FILE;

FILE *fopen(const char *pathname, const char *mode);
int fread(void *ptr, int size, int nmemb, FILE *stream);
int fwrite(const void *ptr, int size, int nmemb, FILE *stream);
int fflush(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
int feof(FILE *stream);
int fclose(FILE *stream);

FILE *fopen(const char *pathname, const char *mode) {
    FILE *fp = (FILE *)malloc(sizeof(FILE));
    if (!fp) {
        return NULL;
    }

    int flags;
    int plus_mode = FALSE;

    // Check for '+' in the mode
    if (strchr(mode, '+')) {
        plus_mode = TRUE;
    }

    switch (mode[0]) {
        case 'r':
            flags = plus_mode ? O_RDWR : O_RDONLY;
            break;
        case 'w':
            flags = plus_mode ? O_RDWR : O_CREAT | O_TRUNC;
            break;
        case 'a':
            flags = plus_mode ? O_RDWR : O_CREAT | O_APPEND;
            break;
        default:
            // Unsupported mode
            free(fp);
            return NULL;
    }

    fp->fd = open(pathname, flags, 0666);
    if (fp->fd < 0) {
        free(fp);
        return NULL;
    }

    fp->pos = 0;
    fp->size = 0;
    fp->mode = mode[0];
    fp->flag = 0;
    fp->lastop = 0;
    fp->eof = FALSE;
    fp->buffer = (char *)malloc(BUFSIZE);
    if (!fp->buffer) {
        free(fp);
        return NULL;
    }

    return fp;
}

int fread(void *ptr, int size, int nmemb, FILE *stream) {
    if (stream->mode != 'r' && stream->mode != 'a') {
        return 0;
    }

    int total = size * nmemb;
    int read_size = 0;
    int read_total = 0;

    while (read_total < total) {
        if (stream->pos == stream->size) {
            stream->size = read(stream->fd, stream->buffer, BUFSIZE);
            if (stream->size == 0) {
                stream->eof = TRUE;
                break;
            }
            stream->pos = 0;
        }

        read_size = total - read_total;
        if (read_size > stream->size - stream->pos) {
            read_size = stream->size - stream->pos;
        }

        memcpy((char *)ptr + read_total, stream->buffer + stream->pos, read_size);
        stream->pos += read_size;
        read_total += read_size;
    }

    return read_total / size;
}

int fwrite(const void *ptr, int size, int nmemb, FILE *stream) {
    if (stream->mode != 'w' && stream->mode != 'a') {
        return 0;
    }

    int total = size * nmemb;
    int write_size = 0;
    int write_total = 0;

    while (write_total < total) {
        if (stream->pos == BUFSIZE) {
            write(stream->fd, stream->buffer, stream->pos);
            stream->pos = 0;
        }

        write_size = total - write_total;
        if (write_size > BUFSIZE - stream->pos) {
            write_size = BUFSIZE - stream->pos;
        }

        memcpy(stream->buffer + stream->pos, (char *)ptr + write_total, write_size);
        stream->pos += write_size;
        write_total += write_size;
    }

    return write_total / size;
}

int fflush(FILE *stream) {
    if (stream->mode != 'w' && stream->mode != 'a') {
        return EOF;
    }

    if (stream->pos > 0) {
        write(stream->fd, stream->buffer, stream->pos);
        stream->pos = 0;
    }

    return 0;
}

int fseek(FILE *stream, long offset, int whence) {
    if (stream->mode != 'r' && stream->mode != 'a') {
        return -1;
    }

    if (whence == SEEK_SET) {
        if (offset < 0) {
            return -1;
        }
        if (offset > stream->size) {
            offset = stream->size;
        }
        stream->pos = offset;
    } else if (whence == SEEK_CUR) {
        if (stream->pos + offset < 0) {
            return -1;
        }
        if (stream->pos + offset > stream->size) {
            stream->pos = stream->size;
        } else {
            stream->pos += offset;
        }
    } else if (whence == SEEK_END) {
        if (offset > 0) {
            return -1;
        }
        stream->pos = stream->size + offset;
    } else {
        return -1;
    }

    return 0;
}

int feof(FILE *stream) {
    return stream->eof;
}

int fclose(FILE *stream) {
    if (stream->mode != 'r' && stream->mode != 'a') {
        return EOF;
    }

    if (stream->mode == 'w' || stream->mode == 'a') {
        fflush(stream);
    }

    close(stream->fd);
    free(stream->buffer);
    free(stream);

    return 0;
}

