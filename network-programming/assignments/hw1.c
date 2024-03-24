#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main()
{
    // 도메인 네임 입력받기 예) www.naver.com
    char domain_name[100];
    scanf("%s", domain_name);

    // hints로 가져올 ip주소 정보를 필터링함.
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    // 소켓타입을 SOCK_STREAM으로 설정함. -> TCP만 필터링. (안하면 같은 주소가
    // 3개씩 나옴.)
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res;

    // res로 도메인 네임에 연결된 ip주소들을 가져옴.
    getaddrinfo(domain_name, NULL, &hints, &res);

    for(struct addrinfo *p = res; p != NULL; p = p->ai_next)
    {
        char output[100];

        // 현재 선택된 주소가 IPv4이면,
        if(p->ai_family == AF_INET)
        {
            // 현재 선택된 주소를 IPv4 포맷으로 변환해서 출력함.
            inet_ntop(p->ai_family,
                      &((struct sockaddr_in *)p->ai_addr)->sin_addr, output,
                      100);

            printf(" - IPv4 : %s\n", output);
        }
        // 현재 선택된 주소가 IPv6이면,
        else
        {
            // 현재 선택된 주소를 IPv6 포맷으로 변환해서 출력함.
            inet_ntop(p->ai_family,
                      &((struct sockaddr_in6 *)p->ai_addr)->sin6_addr, output,
                      100);
            printf(" - IPv6 : %s\n", output);
        }
    }

    // getaddrinfo로 가져온 struct addrinfo는 free해줘야함.
    freeaddrinfo(res);

    return 0;
}