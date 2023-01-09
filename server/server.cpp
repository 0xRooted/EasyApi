#include "server.hpp"

Server::Server(std::string host, int port, bool debug, int max_buff_size, int max_epoll_events)
{
    httpparser = new http();
    myepoll = new epoll(host, port);
    buffer  = new char[max_buff_size];
    myepoll->events = new epoll_event[max_epoll_events];
    memset(buffer, 0, max_buff_size);
    memset(myepoll->events, 0, max_epoll_events);
    this->max_epoll_events = max_epoll_events;
    this->debug = debug;
}

Server::~Server()
{
    delete myepoll;
}

bool Server::init()
{
    return myepoll->epoll_init();
}

void Server::handle(int index)
{
    if (myepoll->events[index].data.fd == myepoll->listen_fd)
    {
        if (myepoll->epoll_accept())
        {
            if (debug)
            {
                std::cout
                    << "[NEW CONNECTION]\n"
                    << "[SOCKET]  [ " << myepoll->client_fd << " ]\n"
                    << "[ADDRESS] [ " << inet_ntoa(myepoll->client_addr.sin_addr) << " ]\n"
                    << "[PORT]    [ " << ntohs(myepoll->client_addr.sin_port) << " ]\n"
                    << std::endl;
            }
            return;
        }
    }
    
    if (myepoll->events[index].events & EPOLLIN)
    {
        switch (myepoll->epoll_read_raw(index, buffer))
        {
            case MSG_DISCONNECTED: {
                if (debug)
                {
                    std::cout
                        << "[SOCKET]  [ " << myepoll->events[index].data.fd << " ]\n"
                        << "[DISCONNECTED]\n"
                        << std::endl;
                }
                myepoll->epoll_del(index);
                return;
            }
            default: break;
        }
        if (strlen(buffer) > 0)
        {
            
            if (debug)
            {
                std::cout
                    << "[SOCKET]  [ " << myepoll->events[index].data.fd << " ]\n"
                    << "[MESSAGE] [ " << buffer << " ]\n"
                    << "[LENGTH]  [ " << strlen(buffer) << " ]\n"
                    << std::endl;
            }
            
            httpparser->init(buffer);
            u->fd = myepoll->events[index].data.fd;
            u->request = httpparser;
            u->con_in = myepoll->get_socket_info(myepoll->events[index].data.fd);
            for (auto &endpoint : endpoints)
            {
                if (httpparser->headers->get("path") == endpoint.first)
                {
                    endpoint.second(u);
                    return;
                }
            }
            
            auto notfound_handler = endpoints.find("/404"); // (u);
            if (notfound_handler != endpoints.end())
                notfound_handler->second(u);
            else
                httpparser->finalize_response(myepoll->events[index].data.fd, httpparser->craft_response_html(404, "<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body></html>"));
        }
    }        
    
    if (myepoll->events[index].events & (EPOLLRDHUP | EPOLLHUP))
    {
        if (debug)
        {
            std::cout
                << "[SOCKET]  [ " << myepoll->events[index].data.fd << " ]\n"
                << "[DISCONNECTED]\n"
                << std::endl;
        }
        myepoll->epoll_del(index);
    }
}

void Server::loop()
{
    while ( (myepoll->nfds = epoll_wait(myepoll->epoll_fd, myepoll->events, max_epoll_events, -1)) > 0 )
    {
        for (int i = 0; i < myepoll->nfds; i++)
            handle(i);
    }
}

void Server::add_endpoint(const std::string& path, void(*func)(user *))
{
    endpoints[path] = func;
}
