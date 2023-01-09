#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <map>

class request_headers
{
    private:
        std::map<std::string, std::string> request;
        std::map<std::string, std::string> parse_request(const std::string& request);
        std::string trim(const std::string& s);
    public:
        request_headers();
        ~request_headers();
        void init(const char *message);
        std::string get(const char * key);
};

class http
{
    private:
        int fd;
        char *message;
    public:
        http();
        ~http();
        request_headers *headers;
        void init(const char *message);
        std::string decodeURIComponent(const std::string& encoded_string);
        std::string craft_response(int status_code, const char *data);
        std::string craft_response_html(int status_code, const char *data);
        std::string craft_response_file(int status_code, const std::string& file_path);
        std::string craft_response_file_html(int status_code, const std::string& file_path);
        std::map<std::string, std::string> parse_query_string(const std::string& query_string);
        std::map<std::string, std::string> parse_post_string(const std::string& post_string);
        void finalize_response(int fd, std::string data);
};

#endif