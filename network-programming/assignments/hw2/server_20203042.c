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

#include <ctype.h>

#include <time.h>

int main()
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // bind 예정

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address); // IP: localhost(127.0.0.1), PORT: 8080

    SOCKET socket_listen; // 클라이언트의 연결 요청을 받을 소켓
    socket_listen = socket(bind_address->ai_family,
                           bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen))
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    // IP, PORT 할당
    if (bind(socket_listen,
             bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);

    // 클라이언트의 연결 요청을 받을 수 있도록 대기
    if (listen(socket_listen, 10) < 0)
    {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    fd_set master; // 어떤 FD에서 데이터가 들어오면 감지할지 세팅하는 fd_set
    FD_ZERO(&master);
    FD_SET(socket_listen, &master); // socket_listen의 readable을 감지할 것임
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");

    while (1)
    {
        fd_set reads;
        reads = master;
        // fd_set에서 감지할 것으로 세팅된 FD에 데이터가 들어올때까지 대기
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        // 리눅스에서 돌아가서 FD를 3부터 시작함(리눅스에서 0, 1, 2는 stdin, stdout, stderr)
        for (i = 3; i <= max_socket; ++i)
        {
            // 어떤 File에 데이터가 들어왔는지 확인
            if (FD_ISSET(i, &reads))
            {
                // 클라이언트에서 연결 요청이 왔으면
                if (i == socket_listen)
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    // 클라이언트의 연결 요청을 받고 연결된 소켓을 리턴
                    SOCKET socket_client = accept(socket_listen,
                                                  (struct sockaddr *)&client_address,
                                                  &client_len);
                    if (!ISVALIDSOCKET(socket_client))
                    {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GETSOCKETERRNO());
                        return 1;
                    }
                    // 새로운 클라이언트 소켓을 fd_set에 추가해서 감지할 수 있도록 함
                    FD_SET(socket_client, &master);
                    printf("socket_client : %d\n", socket_client);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[100];
                    char port_buffer[100];
                    getnameinfo((struct sockaddr *)&client_address,
                                client_len,
                                address_buffer, sizeof(address_buffer), port_buffer, sizeof(port_buffer),
                                0);
                    printf("New connection from %s %s\n", address_buffer, port_buffer);

                    // Client에게 안내문 보내기
                    const char *message = "1: 현재시간, 2: 계산(예: \"2 1+2\")\n";
                    send(socket_client, message, strlen(message), 0);
                }
                // 클라이언트에서 데이터가 왔으면
                else
                {
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    // 클라이언트에서 quit을 보내면 연결 종료
                    // bytes_received가 0일때를 체크 안했으므로
                    // 클라이언트에서 Ctrl C로 먼저 연결을 끊어도 서버에서는 연결을 끊지 않음.
                    if (strncmp(read, "quit", 4) == 0)
                    {
                        printf("'quit' from client %d\n", i - 3);
                        FD_CLR(i, &master);
                        printf("\t : response done.\n");
                        CLOSESOCKET(i);
                        continue;
                    }
                    // 현재 시간 요청이면
                    if (read[0] == '1')
                    {
                        printf("'current time' from client %d\n", i - 3);
                        // 현재 시간을 구해서 보냄
                        time_t rawtime;
                        struct tm *timeinfo;
                        char buffer[80];
                        time(&rawtime);
                        timeinfo = localtime(&rawtime);

                        strftime(buffer, 80, "현재시간 : %Y년 %m월 %d일 %H시 %M분 %S초\n", timeinfo);

                        send(i, buffer, strlen(buffer), 0);
                        printf("\t : response done.\n");
                    }
                    // 계산 요청이면
                    else if (read[0] == '2')
                    {
                        printf("'calculation' from client %d\n", i - 3);
                        // 연산자, 피연산자 구분해 계산
                        int num1, num2;
                        char op;
                        int j = sscanf(read + 1, "%d %c %d", &num1, &op, &num2);
                        if (j < 3)
                            continue;

                        int result;
                        switch (op)
                        {
                        case '+':
                            result = num1 + num2;
                            break;
                        case '-':
                            result = num1 - num2;
                            break;
                        case '*':
                            result = num1 * num2;
                            break;
                        case '/':
                            result = num1 / num2;
                            break;
                        default:
                            result = 0;
                            break;
                        }

                        char buffer[80];
                        sprintf(buffer, "%d %c %d = %d\n", num1, op, num2, result);
                        send(i, buffer, strlen(buffer), 0);

                        printf("\t : response done.\n");
                    }
                }
            }
        }
    }

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

    printf("Finished.\n");

    return 0;
}
