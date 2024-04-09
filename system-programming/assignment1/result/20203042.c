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
    // str_data는 "10011101" 같은 문자열이 들어있음.
    char *str_data = (char *)malloc(bits_len);
    fread(str_data, 1, bits_len, file);
    fclose(file);
    file = NULL;

    printf("BUFSIZ : %d\n\n", bits_len);

    int bytes_len = bits_len / 8;

    char *copied_str_data = (char *)malloc(bits_len);
    memcpy(copied_str_data, str_data, bits_len);

    // 메모리(memory)에 바이너리 데이터를 리틀 엔디안을 고려해 넣을것임
    void *memory = malloc(bytes_len);
    set_memory(memory, copied_str_data, bits_len);

    // 1바이트 자료형은 엔디안의 영향을 받지 않음.
    // -> 엔디안은 멀티 바이트 자료형일때 그 안에서 바이트 순서가 바뀌는 것이기 때문임
    // C의 배열은 어떤 메모리에 저장되어도 원소들의 주소가 오름차순으로 연속적으로 지정된다.
    // -> 힙에 저장되든, 스택에 저장되든, 리틀엔디안이든 빅엔디안이든 상관없이 배열의 원소 주소는 오름차순으로 연속적이다.
    // 과제에서 데이터 하나의 크기를 넘어서는 비트를 줬을떄, 그걸 배열로 해석하면 그대로 읽으면 된다.
    // 실제로 1바이트 자료형 char의 배열을 dump_memory로 확인했을때, 순서 그대로 메모리에 들어간 것을 확인했다.
    // 4바이트 자료형 int의 배열은 dump_memory로 확인했을때, 배열의 순서는 그대로지만,
    // 각각의 배열 원소가 리틀엔디안의 영향을 받아서 바이트 단위로 거꾸로 된 것을 확인 할 수 있었다.

    // signed char, unsigned char은 1바이트 자료형이기 때문에 엔디안의 영향을 받지않고, 그대로 출력하면 된다.
    // signed와 unsigned는 맨 앞의 비트를 부호비트로 쓰냐 안쓰냐의 차이기 때문에, 값이 달라진다.
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
    // 4바이트 내에서 리틀엔디안에 의해 바이트가 거꾸로 저장된다.
    // signed int와 unsigned int는 맨 앞 비트를 부호비트로 쓰느냐의 차이기 때문에 값에 차이가 있고,
    // float는 부호 1비트, 지수 8비트, 가수 23비트로 구성되어있다.

    // 리틀엔디안을 고려해서 4바이트씩 끊어서 문자열을 거꾸로 처리해준뒤, 메모리에 넣어준다.
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

    // 8바이트 자료형인 double은 마찬가지로 리틀엔디안에 의해 바이트가 거꾸로 저장된다.
    // double은 부호 1비트, 지수 11비트, 가수 52비트로 구성되어있다.

    memcpy(copied_str_data, str_data, bits_len);

    for (int i = 0; i < bytes_len / 8; i++)
        reverse_str_in_bytes(copied_str_data + 8 * i, 8);

    set_memory(memory, copied_str_data, bits_len);

    printf("7. double : ");
    for (int i = 0; i < bytes_len / 8; i++)
        printf("%lf ", ((double *)memory)[i]);
    printf("\n");

    free(memory);
    free(str_data);
    free(copied_str_data);

    memory = NULL;
    str_data = NULL;
    copied_str_data = NULL;

    return 0;
}