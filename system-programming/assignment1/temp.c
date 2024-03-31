#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 64

int main()
{
    char binary[SIZE + 1];
    unsigned long long int value = 0;

    printf("Enter a 64-bit binary string: ");
    scanf("%s", binary);

    if(strlen(binary) != SIZE)
    {
        printf(
            "Invalid binary string length. Please enter a 64-bit binary "
            "string.\n");
        return 1;
    }

    for(int i = 0; i < SIZE; i++)
    {
        if(binary[i] == '1')
        {
            value |= (1ULL << (SIZE - 1 - i));
        }
        else if(binary[i] != '0')
        {
            printf(
                "Invalid binary string. Please enter a valid binary string.\n");
            return 1;
        }
    }

    printf("Value stored in memory: %llu\n", value);

    return 0;
}