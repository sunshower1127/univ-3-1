#include "20203042.h"

int main()
{
    FILE *file = fopen("input", "r");
    if (file == NULL)
        return 1;

    // Get the file size
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory on the heap
    char *data = (char *)malloc(file_size + 1);
    data[file_size] = '\0'; // Null-terminate the string
    if (data == NULL)
    {
        printf("Failed to allocate memory.\n");
        fclose(file);
        return 1;
    }

    // Read the file contents into the allocated memory
    fread(data, 1, file_size, file);

    // Close the file
    fclose(file);

    printf("BUFSIZ : %d\n\n", file_size);
    printf("input : %s\n\n", data);
    int reserve = file_size / 8;
    printf("reserve : %d\n\n", reserve);
    unsigned char *temp = malloc(reserve);
    memset(temp, 0, reserve);

    // for (int i = 0; i < reserve; i++)
    //     for (int j = 0; j < 8; j++)
    //         if (data[i * 8 + j] == '1')
    //             temp[i] |= (1 << (7 - j));

    // 8비트 단위로 읽기
    // 1. 문자를 8비트 단위로 자름.
    // 2. 메모리에 거꾸로 넣음.

    set_memory(temp, data, file_size);

    printf("1. signed char : ");

    for (int i = 0; i < reserve; i++)
        printf("%d ", ((char *)temp)[i]);
    printf("\n");

    printf("2. ASCII codes : ");
    for (int i = 0; i < reserve; i++)
    {
        if (((char *)temp)[i] < 0 || ((char *)temp)[i] > 126)
            printf(".");
        else
            printf("%c ", ((char *)temp)[i]);
    }
    printf("\n");

    printf("3. unsigned char : ");
    for (int i = 0; i < reserve; i++)
        printf("%ud ", ((unsigned char *)temp)[i]);
    printf("\n");

    // 32비트 -> 4바이트 단위로 읽기

    for (int i = 0; i < reserve / 4; i++)
        reverse_str(data + 4 * i, 4);
    set_memory(temp, data, file_size);

    printf("4. signed int : ");
    for (int i = 0; i < reserve / 4; i++)
        printf("%d ", ((int *)temp)[i]);
    printf("\n");

    printf("5. unsigned int : ");
    for (int i = 0; i < reserve / 4; i++)
        printf("%ud ", ((unsigned int *)temp)[i]);
    printf("\n");

    printf("6. signed float : ");
    for (int i = 0; i < reserve / 4; i++)
        printf("%.4f ", ((float *)temp)[i]);
    printf("\n");

    // 64비트

    for (int i = 0; i < reserve / 8; i++)
        reverse_str(data + 8 * i, 8);
    set_memory(temp, data, file_size);

    printf("7. signed double : ");
    printf("%.4lf ", ((double *)temp)[0]);
    printf("\n");

    // Free the allocated memory
    free(temp);
    free(data);

    return 0;
}