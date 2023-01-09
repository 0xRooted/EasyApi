#include "epoll.hpp"
#include <iostream>

epoll::epoll(std::string host, int port)
{
    epoll_fd    = 0;
    listen_fd   = 0;
    client_fd   = 0;
    nfds        = 0;
    memset(&ev, 0, sizeof(ev));
    memset(&events, 0, sizeof(events));
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    clen         = 0;
    slen         = 0;
    saddress    = host;
    sport       = port;
}

epoll::epoll(const char *host, int port)
{
    epoll_fd    = 0;
    listen_fd   = 0;
    client_fd   = 0;
    nfds        = 0;
    memset(&ev, 0, sizeof(ev));
    memset(&events, 0, sizeof(events));
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    clen         = 0;
    slen         = 0;
    saddress    = std::string(host);
    sport       = port;
}

epoll::epoll(char *host, int port)
{
    epoll_fd    = 0;
    listen_fd   = 0;
    client_fd   = 0;
    nfds        = 0;
    memset(&ev, 0, sizeof(ev));
    memset(&events, 0, sizeof(events));
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    clen         = 0;
    slen         = 0;
    saddress    = std::string(host);
    sport       = port;
}

epoll::~epoll()
{
    
}

edata::edata()
{

}

edata::~edata()
{

}

bool epoll::epoll_init()
{
    if (!this->setup())
        return false;

    if ((this->epoll_fd = epoll_create(1)) == -1)
        return false;
    
    if (!epoll_add(this->listen_fd))
        return false;

    this->slen = sizeof(server_addr);
    this->clen = sizeof(client_addr);

    return true;
}

bool epoll::epoll_add(SOCK fd, uint32_t events)
{
    struct epoll_event ev;
    events != 0 ? ev.events = events : ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    ev.data.fd = fd;
    return epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1 ? false : true;
}

bool epoll::epoll_del(int index)
{
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, events[index].data.fd, NULL) == -1)
        return false;

    return epoll_close(index);
}

bool epoll::epoll_close(int index)
{
    return close(events[index].data.fd) == 0 ? true : false;
}

bool epoll::epoll_accept()
{
    if ((client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &clen)) < 0)
        return false;
    
    if (!set_nonblocking(client_fd))
        return false;
    
    if (!epoll_add(client_fd, EPOLLIN | EPOLLET))
        return false;
    
    return true;
}

int epoll::epoll_read_raw(int index, char *buffer, size_t rbytes)
{
    memset(buffer, 0, rbytes);
    size_t bread = read(events[index].data.fd, buffer, rbytes);

    switch (bread)
    {
        case 0:
            return MSG_DISCONNECTED;
        
        case -1:
            return MSG_ERROR;

        default:
            break;
    }
    return bread;
}

int epoll::epoll_read(int index, char *buffer, size_t rbytes)
{
    memset(buffer, 0, rbytes);
    size_t bread = read(events[index].data.fd, buffer, rbytes);

    switch (bread)
    {
        case 0:
            return MSG_DISCONNECTED;
        
        case -1:
            return MSG_ERROR;

        default:
            break;
    }
    
    epoll_buff_clean(buffer);

    return bread;
}

void epoll::epoll_buff_clean(char *buffer)
{
    for (size_t i = 0; i < strlen(buffer); i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\r')
            buffer[i] = 0;
    }
}

void epoll::epoll_write(int index, char *buffer)
{
    write(events[index].data.fd, buffer, strlen(buffer) + 1);
}

/* ----------------------------------------------------------------------------------------------------------------------- */

socks::socks()
{
    
}

socks::~socks()
{
    
}

bool socks::setup()
{
    const int enable = 1;
    if ((this->listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return false;

    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(this->sport);
    this->server_addr.sin_addr.s_addr = inet_addr(this->saddress.c_str());

    if (setsockopt(this->listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)) < 0)
        return false;

    if (bind(this->listen_fd, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr)) == -1)
        return false;
    
    if (!set_nonblocking(this->listen_fd))
        return false;

    return listen(this->listen_fd, SOMAXCONN) == -1 ? false : true;
}

bool socks::set_nonblocking(SOCK fd)
{
    return fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) == -1 ? false : true;
}

socket_info socks::get_socket_info(int socket)
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(socket, (struct sockaddr *)&addr, &addr_size);
    socket_info info;
    info.ip = inet_ntoa(addr.sin_addr);
    info.port = ntohs(addr.sin_port);
    return info;
}