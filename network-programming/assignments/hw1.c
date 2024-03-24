#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main()
{
    

    char domain_name[100];
    scanf("%s", domain_name);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res;
    
    getaddrinfo(domain_name, NULL, &hints, &res);

    for (struct addrinfo *p = res; p != NULL; p = p->ai_next)
    {

        char result[100];

        if (p->ai_family == AF_INET)
        {
            inet_ntop(p->ai_family, &((struct sockaddr_in *)p->ai_addr)->sin_addr, result, 100);
            printf(" - IPv4 : %s\n", result);
        }
        else
        {
            inet_ntop(p->ai_family, &((struct sockaddr_in6 *)p->ai_addr)->sin6_addr, result, 100);
            printf(" - IPv6 : %s\n", result);
        }
    }

    freeaddrinfo(res);

    return 0;
}