#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *file = fopen("input", "r");
    if(file == NULL) return 1;

    // Get the file size
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory on the heap
    char *data = (char *)malloc(file_size + 1);
    data[file_size] = '\0';  // Null-terminate the string
    if(data == NULL)
    {
        printf("Failed to allocate memory.\n");
        fclose(file);
        return 1;
    }

    // Read the file contents into the allocated memory
    fread(data, 1, file_size, file);

    // Close the file
    fclose(file);

    // Print the binary data
    printf("BUFSIZ : %d\n\n", file_size);
    printf("input : %s\n\n", data);
    int reserve = (int)ceil(file_size / 8);
    printf("reserve : %d\n\n", reserve);
    void *temp = calloc(reserve, 1);
    for(int i = 0; i < reserve; i++)
        for(int j = 0; j < 8; j++)
            if(data[i * 8 + j] == '1')
            {
                *((char *)temp + i) |= (1 << (7 - j));
            }

    int *value = (int *)temp;
    printf("3. signed int : %d %d \n", value[0], value[1]);
    // Free the allocated memory
    free(temp);
    free(data);

    return 0;
}