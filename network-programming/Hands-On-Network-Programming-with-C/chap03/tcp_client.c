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

#if defined(_WIN32)
#include <conio.h>
#endif

int main(int argc, char *argv[])
{

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    if (argc < 3)
    {
        fprintf(stderr, "usage: tcp_client hostname port\n");
        return 1;
    }

    printf("Configuring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    // getaddrinfo
    //   setting = hints(addrinfo)
    //   input = domain_name, port
    //   output = ip_address(addrinfo)
    if (getaddrinfo(argv[1], argv[2], &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    // getnameinfo
    //   setting -> 첫번째 output 세팅.
    //     NULL -> 호스트이름 반환
    //     NI_NAMEREQD -> 호스트이름을 반드시 만환
    //     NI_NOFQDN -> 도메인이름 말고 ...
    //     NI_NUMERICHOST -> ip주소 반환
    //     NI_NUMERICSERV -> 포트번호 반환
    //     NI_DGRAM -> UDP 서비스 이름 반환

    //   input = ip_address(addrinfo*->ai_addr,ai_addrlen)
    //   output = ip_address(char*), port(char*)["http"]
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);

    printf("Creating socket...\n");
    SOCKET socket_peer;
    // socket -> 프로토콜 선택. IPv4 & TCP
    //   input = ip_addr(addrinfo)의
    //     family(int)=AF_INET|AF_INET6 ... -> IP버전
    //     socktype(int)=SOCK_STREAM|SOCK_DGRAM ... -> TCP/UDP
    //     protocol(int)=0 -> default -> 보통은 0으로 놓고 알아서 선택하게 함.
    //   ouput = SOCKET(int) -> 소켓 번호 리턴
    socket_peer = socket(peer_address->ai_family,
                         peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_peer))
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Connecting...\n");
    // connect -> 상대의 ip주소를 통해 연결함.
    //   input
    //     socket
    //     ip주소 = addr_info.ai_addr, ai_addrlen
    //   output = 성공시 0, 실패시 -1 리턴
    if (connect(socket_peer,
                peer_address->ai_addr, peer_address->ai_addrlen))
    {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(peer_address);

    printf("Connected.\n");
    printf("To send data, enter text followed by enter.\n");

    while (1)
    {

        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads);
#if !defined(_WIN32)
        FD_SET(0, &reads);
#endif

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms

        if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0)
        {
            // select -> 여러 소켓을 한번에 관리할 수 있게 해줌.
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        if (FD_ISSET(socket_peer, &reads))
        {
            char read[4096];
            // recv -> receive
            //   setting = flags
            //   input = SOCKET
            //   output = 받을 버퍼(char*), 버퍼크기(size_t),
            //     받은 바이트 수(ssize_t) 리턴
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if (bytes_received < 1)
            {
                printf("Connection closed by peer.\n");
                break;
            }
            printf("Received (%d bytes): %.*s",
                   bytes_received, bytes_received, read);
        }

#if defined(_WIN32)
        if (_kbhit())
        {
#else
        if (FD_ISSET(0, &reads))
        {
#endif
            char read[4096];
            if (!fgets(read, 4096, stdin))
                break;
            printf("Sending: %s", read);
            // send
            //   setting = flags
            //   input = SOCKET, 보낼 버퍼(char*), 버퍼크기(size_t)
            //   output = 보낸 바이트 수(ssize_t)
            int bytes_sent = send(socket_peer, read, strlen(read), 0);
            printf("Sent %d bytes.\n", bytes_sent);
        }
    } // end while(1)

    printf("Closing socket...\n");
    CLOSESOCKET(socket_peer);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}
