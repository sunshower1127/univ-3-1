
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

#include <stdio.h>
#include <string.h>

int main()
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM; // TCP
    struct addrinfo *peer_address;

    if (getaddrinfo("localhost", "8080", &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];

    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer,
                sizeof(address_buffer), service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);

    // 서버와 연결할 소켓 생성
    SOCKET socket_peer;
    socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype,
                         peer_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_peer))
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    // 서버에 연결 요청
    if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen))
    {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(peer_address);

    printf("Connected.\n");

    while (1)
    {
        fd_set reads; // 어떤 FD에서 데이터가 들어오면 감지할지 세팅하는 fd_set
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads); // 서버에서 데이터가 들어오면 감지할 것임
        FD_SET(0, &reads);           // stdin의 FD를 넣음. -> 사용자가 키보드로 입력한 것을 받기 위함
        struct timeval timeout;      // timeout 설정
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms = 0.1s

        // 감지 설정된 File에 데이터 들어올때까지 대기
        if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        // 서버에 데이터가 들어오면
        if (FD_ISSET(socket_peer, &reads))
        {
            char read[4096];
            // 데이터 받기
            int bytes_received = recv(socket_peer, read, 4096, 0);
            // 상대가 소켓을 close하거나 에러가 발생하면 종료
            if (bytes_received < 1)
            {
                printf("Connection closed by peer.\n");
                break;
            }
            printf("%.*s", bytes_received, read);
        }

        // 리눅스 기준 stdin에서 데이터가 들어오면
        if (FD_ISSET(0, &reads))
        {
            char read[4096];
            // 유저 인풋 받기
            if (!fgets(read, 4096, stdin))
                break;
            // 받은 인풋 그대로 서버로 보내기
            int bytes_sent = send(socket_peer, read, strlen(read), 0);
            if (bytes_sent < 1)
            {
                printf("Connection close.\n");
                break;
            }
        }
    }

    printf("Closing socket...\n");
    CLOSESOCKET(socket_peer);

    printf("Finished.\n");
    return 0;
}
