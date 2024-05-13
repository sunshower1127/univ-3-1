#include "mystdio.h"
#include <assert.h>

void test_fflush()
{
    char *filename = "testfile.txt";
    char *write_data = "Hello, World!";
    char read_data[50];

    FILE *file = fopen(filename, "w+");
    assert(file != NULL);

    // Write data to the file
    assert(fwrite(write_data, sizeof(char), strlen(write_data), file) == strlen(write_data));

    // Flush the buffer
    assert(fflush(file) == 0);

    file = fopen(filename, "r");
    assert(file != NULL);

    // Read the data from the file
    assert(fread(read_data, sizeof(char), sizeof(read_data), file) == strlen(write_data));

    // Check that the read data matches the written data
    assert(strncmp(read_data, write_data, strlen(write_data)) == 0);

    printf("fflush test passed\n");
}

int main()
{
    test_fflush();
    return 0;
}