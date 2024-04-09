#include "20203042.h"

int main()
{
    // input 파일 열기
    FILE *file = fopen("input", "r");

    // input 파일에 있는 데이터 길이 구하기
    // -> fseek으로 파일 끝으로 이동 후 ftell로 길이 구함.
    fseek(file, 0, SEEK_END);
    int bits_len = ftell(file);
    fseek(file, 0, SEEK_SET);

    // input 파일 데이터를 str_data에 넣음.
    // str_data는 "10011101" 같은 input 문자열이 들어있음.
    char *str_data = (char *)malloc(bits_len);
    fread(str_data, 1, bits_len, file);
    fclose(file);
    file = NULL;

    int bytes_len = bits_len / 8;

    // 엔디안에 따라서 변환될 바이너리 문자열
    char *copied_str_data = (char *)malloc(bits_len);
    memcpy(copied_str_data, str_data, bits_len);

    // 1비트씩 저장할 메모리
    void *memory = malloc(bytes_len);

    // 1바이트 자료형인 char, unsigned char 출력
    // 그냥 그대로 쓰고 읽으면 됨
    set_memory(memory, copied_str_data, bits_len);

    printf("1. signed char : ");

    for (int i = 0; i < bytes_len; i++)
        printf("%d ", ((char *)memory)[i]);
    printf("\n");

    printf("2. ASCII codes : ");
    for (int i = 0; i < bytes_len; i++)
    {
        // 출력할 수 있는 값에서 벗어난 아스키 코드값은 .으로 처리한다.
        if (((char *)memory)[i] < 0 || ((char *)memory)[i] > 126)
            printf(".");
        else
            printf("%c ", ((char *)memory)[i]);
    }
    printf("\n");

    printf("3. unsigned char : ");
    for (int i = 0; i < bytes_len; i++)
        printf("%ud ", ((unsigned char *)memory)[i]);
    printf("\n");

    // 4바이트 자료형인 signed int, unsigned int, float은
    // 리틀엔디안에 따라서 원래 4바이트 데이터가 바이트 단위로 쪼개져서 저장되므로
    // 넣을 바이너리 데이터 문자열을 4바이트 단위로 거꾸로 바꿔주고 메모리에 넣어준다.
    memcpy(copied_str_data, str_data, bits_len);

    for (int i = 0; i < bytes_len / 4; i++)
        reverse_str_in_bytes(copied_str_data + 4 * i, 4);

    set_memory(memory, copied_str_data, bits_len);

    printf("4. signed int : ");
    for (int i = 0; i < bytes_len / 4; i++)
        printf("%d ", ((int *)memory)[i]);
    printf("\n");

    printf("5. unsigned int : ");
    for (int i = 0; i < bytes_len / 4; i++)
        printf("%ud ", ((unsigned int *)memory)[i]);
    printf("\n");

    printf("6. float : ");
    for (int i = 0; i < bytes_len / 4; i++)
        printf("%.4f ", ((float *)memory)[i]);
    printf("\n");

    // 8바이트 자료형인 double은 8바이트 단위로 거꾸로 변환하고 저장.
    memcpy(copied_str_data, str_data, bits_len);

    for (int i = 0; i < bytes_len / 8; i++)
        reverse_str_in_bytes(copied_str_data + 8 * i, 8);

    set_memory(memory, copied_str_data, bits_len);

    printf("7. double : ");
    for (int i = 0; i < bytes_len / 8; i++)
        printf("%.4lf ", ((double *)memory)[i]);
    printf("\n");

    free(memory);
    free(str_data);
    free(copied_str_data);

    memory = NULL;
    str_data = NULL;
    copied_str_data = NULL;

    return 0;
}