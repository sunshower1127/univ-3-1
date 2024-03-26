#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int main()
{
    struct sockaddr_in sa;  // input your address
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(80);                           // for example
    inet_pton(AF_INET, "23.51.28.244", &sa.sin_addr);  // for example

    if(getnameinfo((struct sockaddr *)&sa, sizeof(struct sockaddr_in), hbuf,
                   sizeof(hbuf), sbuf, sizeof(sbuf), 0))
    {
        printf("Could not resolve hostname for this address\n");
    }
    else
    {
        printf("host=%s, serv=%s\n", hbuf, sbuf);
    }

    return 0;
}