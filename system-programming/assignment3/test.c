#include "mystdio.h"
#include <assert.h>

int main()
{
    // Test stdin
    FILE *in = stdin;
    assert(in != NULL);
    assert(in->fd == 0);
    assert(in->mode == 'r');

    // Test stdout
    FILE *out = stdout;
    assert(out != NULL);
    assert(out->fd == 1);
    assert(out->mode == 'w');

    // Test stderr
    FILE *err = stderr;
    assert(err != NULL);
    assert(err->fd == 2);
    assert(err->mode == 'w');

    // Test writing to stdout
    const char *msg = "Hello, World!\n";
    int written = fwrite(msg, sizeof(char), strlen(msg), stdout);
    assert(written == strlen(msg));

    // Test writing to stderr
    const char *errmsg = "This is an error message.\n";
    written = fwrite(errmsg, sizeof(char), strlen(errmsg), err);
    assert(written == strlen(errmsg));

    return 0;
}