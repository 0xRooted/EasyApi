#ifndef SERVER_HPP
#define SERVER_HPP

#include "../epoll/epoll.hpp"
#include "../http/http.hpp"
#include <thread>
#include <iostream>
#include <functional>

struct user
{
    int fd;
    socket_info con_in;
    http *request;
};

class Server
{
    private:
        http  *httpparser      = nullptr;
        user  *u               = new user();
        char  *buffer          = nullptr;
        int   max_epoll_events = 0;
        std::map<std::string, void(*)(user *)> endpoints;
    public:
        epoll *myepoll         = nullptr;
        bool   debug           = false;
        Server(std::string host = std::string("0.0.0.0"), int port = 80, bool debug = false, int max_buff_size = 1024, int max_epoll_events = 10000);
        ~Server();
        bool init();
        void loop();
        void handle(int index);
        void add_endpoint(const std::string& path, void(*func)(user *));
};

#endif