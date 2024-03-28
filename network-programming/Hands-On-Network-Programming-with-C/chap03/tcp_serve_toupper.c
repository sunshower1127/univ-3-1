/*
 * MIT License
 *
 * Copyright (c) 2018 Lewis Van Winkle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "chap03.h"
#include <ctype.h>

#include <stdio.h>

int main()
{

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    printf("Creating socket...\n");
    // socket 만드는 법
    // 1. 서버 : socket(자기자신) -> accept()로 연결요청한 반대쪽 소켓 얻어오기
    // 2. 클라이언트 : socket(상대방) -> connect()로 연결요청
    // socket은 3부터 시작해서 1씩 증가함. 0, 1, 2는 stdin, stdout, stderr 이기 때문.
    // socket에서 recv와 send는 항상 상대소켓에게 행해지고, 상대도 상대소켓에서 받게됨.
    // socket은 콘센트다. client는 socket()으로 규격에 맞는 콘센트를 만들고, connect로 서버와 선을 연결함.
    // server는 socket()으로 규격에 맞는 콘센트를 만들고, bind로 내 위치를 정하고, listen으로 대기함.
    // listen으로 대기중인 server는 상대 client에게서 연결 요청이 오면, 그 내용이 server FD에 저장이 되고,
    // fd_set에서 해당 비트를 1로 바꿈.
    // 그래서 바로 server FD에서 accept로 읽어와서 연결을 수락할 수 있는거임.
    // 그러면 이제 상대 client 소켓을 얻을 수 있고, 거기서 오는 정보들은 client socket FD에 저장함.
    // 만약 정보가 오면, fd_set에서 비트를 1로 바꾸고, ISSET에서 감지하고, 감지하면 recv로 읽어올 수 있음.

    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
                           bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen))
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    printf("socket_listen: %d\n", socket_listen);

    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
             bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0)
    {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    // fd_set은 fd들을 관리함.
    // FD_SET으로 등록 안한 애들은 select에서 무시됨. -> FD_SET(0, &master)을 하지 않으면 stdin은 무시됨.
    // select read는 뭔가 파일에서 읽어올 게 있으면 해당 비트를 1로 바꿈 -> FD_ISSET으로 확인 가능.
    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");

    while (1)
    {
        fd_set reads;
        reads = master;
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        for (i = 3; i <= max_socket; ++i)
        {
            // FD_ISSET이 1일 때
            //
            if (FD_ISSET(i, &reads))
            {
                printf("FD_ISSET : %d \n", i);

                if (i == socket_listen)
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen,
                                                  (struct sockaddr *)&client_address,
                                                  &client_len);
                    if (!ISVALIDSOCKET(socket_client))
                    {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    printf("socket_client : %d\n", socket_client);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[100];
                    getnameinfo((struct sockaddr *)&client_address,
                                client_len,
                                address_buffer, sizeof(address_buffer), 0, 0,
                                NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);
                }
                else
                {
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1)
                    {
                        FD_CLR(i, &master);
                        printf("CLOSESOCKET : %d \n", i);
                        CLOSESOCKET(i);
                        continue;
                    }

                    int j;
                    for (j = 0; j < bytes_received; ++j)
                        read[j] = toupper(read[j]);
                    send(i, read, bytes_received, 0);
                }

            } // if FD_ISSET
        }     // for i to max_socket
    }         // while(1)

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");

    return 0;
}
