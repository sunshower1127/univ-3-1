#include <time.h>
#include <stdio.h>

int main()
{
    // // 한국어로 설정
    // setlocale(LC_ALL, "ko_KR.utf8");

    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // 시간을 "YYYY년 MM월 DD일 HH시 MM분 SS초" 형식으로 출력
    strftime(buffer, 80, "%Y년 %m월 %d일 %H시 %M분 %S초", timeinfo);
    puts(buffer);

    return 0;
}