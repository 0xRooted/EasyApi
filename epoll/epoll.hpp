#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <string>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


typedef int SOCK;

enum Messages
{
    MSG_ERROR = -1,
    MSG_DISCONNECTED = -0xDEAD,
    MSG_OK
};

struct socket_info
{
    std::string ip;
    int port;
};

class socks
{
    public:
        socks();
        ~socks();
        sockaddr_in         server_addr         ;
        std::string         saddress            ;
        int                 sport               ;
        SOCK listen_fd                          ;
        socklen_t           slen                ;
        bool setup()                            ;
        bool set_nonblocking(SOCK fd)           ;
        socket_info get_socket_info(int socket) ;
};

class edata
{
    public:
        edata()                         ;
        ~edata()                        ;
        SOCK                epoll_fd    ;
        SOCK                client_fd   ;
        int                 nfds        ;
        epoll_event         ev          ;
        epoll_event         *events     ;
        sockaddr_in         client_addr ;
        socklen_t           clen        ;
};

class epoll : public edata, public socks
{
    public:
        epoll(std::string host, int port);
        epoll(const char *host, int port);
        epoll(char *host, int port);
        ~epoll();
        bool epoll_init();
        bool epoll_add(SOCK fd, uint32_t events = 0);
        bool epoll_del(int index);
        bool epoll_close(int index);
        int  epoll_read(int index, char *buffer, size_t rbytes = 1024);
        int  epoll_read_raw(int index, char *buffer, size_t rbytes = 1024);
        void epoll_buff_clean(char *buffer);
        void epoll_write(int index, char *buffer);
        bool epoll_accept();
};

#endif