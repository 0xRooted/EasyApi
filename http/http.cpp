#include "http.hpp"

request_headers::request_headers()
{
}

request_headers::~request_headers()
{
}

void request_headers::init(const char *message)
{
    request = parse_request(message);
}

std::map<std::string, std::string> request_headers::parse_request(const std::string &request)
{
    std::map<std::string, std::string> request_info;

    std::stringstream ss(request);
    std::string line;

    // Parse the first line (request line)
    std::getline(ss, line);
    std::stringstream request_line(line);
    request_info["method"] = "";
    request_info["path"] = "";
    request_info["path_full"] = "";
    request_info["query_string"] = "";
    request_info["version"] = "";
    request_line >> request_info["method"] >> request_info["path_full"] >> request_info["version"];

    // Parse the headers
    while (std::getline(ss, line) && line != "\r")
    {
        // Find the position of the ':' character
        size_t pos = line.find(':');

        // Extract the key and value from the header string
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Remove leading and trailing whitespace from the key and value
        key = trim(key);
        value = trim(value);

        // Add the key and value to the request info map
        request_info[key] = value;
    }

    // Check if the request has a POST body
    if (request_info["method"] == "POST")
    {
        // Read the POST body
        std::stringstream post_data;
        post_data << ss.rdbuf();
        std::string post_body = post_data.str();

        // Add the POST body to the request info map
        request_info["post_body"] = post_body;
    }

    auto pos = request_info["path_full"].find('?');
    if (pos != std::string::npos)
    {
        request_info["path"] = request_info["path_full"].substr(0, pos);
        request_info["query_string"] = request_info["path_full"].substr(pos + 1);
    }
    else
        request_info["path"] = request_info["path_full"];

    return request_info;
}

std::map<std::string, std::string> http::parse_query_string(const std::string &query_string)
{
    std::map<std::string, std::string> params;

    std::istringstream iss(query_string);
    std::string pair;
    while (std::getline(iss, pair, '&'))
    {
        size_t pos = pair.find('=');
        if (pos != std::string::npos)
        {
            std::string key = decodeURIComponent(pair.substr(0, pos));
            std::string value = decodeURIComponent(pair.substr(pos + 1));
            params[key] = value;
        }
    }

    return params;
}

std::map<std::string, std::string> http::parse_post_string(const std::string &post_string)
{
    std::map<std::string, std::string> params;

    std::istringstream iss(post_string);
    std::string pair;
    while (std::getline(iss, pair, '&'))
    {
        size_t pos = pair.find('=');
        if (pos != std::string::npos)
        {
            std::string key = decodeURIComponent(pair.substr(0, pos));
            std::string value = decodeURIComponent(pair.substr(pos + 1));
            params[key] = value;
        }
    }

    return params;
}

std::string request_headers::trim(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

std::string http::craft_response(int status_code, const char *data)
{
    std::string status_string;
    switch (status_code)
    {
    case 200:
        status_string = "200 OK";
        break;
    case 404:
        status_string = "404 Not Found";
        break;
    // Add additional status codes as needed
    default:
        status_string = "500 Internal Server Error";
        break;
    }

    std::ostringstream response;
    response << "HTTP/1.1 " << status_string << "\r\n"
             << "Content-Length: " << std::string(data).length() << "\r\n"
             << "Content-Type: text/plain\r\n"
             << "\r\n"
             << data;

    return response.str();
}

std::string http::craft_response_html(int status_code, const char *data)
{
    std::string status_string;
    switch (status_code)
    {
    case 200:
        status_string = "200 OK";
        break;
    case 404:
        status_string = "404 Not Found";
        break;
    // Add additional status codes as needed
    default:
        status_string = "500 Internal Server Error";
        break;
    }

    std::ostringstream response;
    response << "HTTP/1.1 " << status_string << "\r\n"
             << "Content-Length: " << std::string(data).length() << "\r\n"
             << "Content-Type: text/html\r\n"
             << "\r\n"
             << data;

    return response.str();
}

std::string http::craft_response_file(int status_code, const std::string &file_path)
{
    std::string status_line, body;

    // Set the status line based on the status code
    switch (status_code)
    {
    case 200:
        status_line = "HTTP/1.1 200 OK\r\n";
        break;
    case 404:
        status_line = "HTTP/1.1 404 Not Found\r\n";
        break;
    default:
        status_line = "HTTP/1.1 500 Internal Server Error\r\n";
        break;
    }

    // Read the contents of the file into the body
    std::ifstream file(file_path);
    if (file.is_open())
        body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Build the response
    std::string response = status_line + "Content-Length: " + std::to_string(body.size()) + "\r\n" + "Content-Type: text/plain\r\n\r\n" + body;

    return response;
}

std::string http::craft_response_file_html(int status_code, const std::string &file_path)
{
    std::string status_line, body;

    // Set the status line based on the status code
    switch (status_code)
    {
    case 200:
        status_line = "HTTP/1.1 200 OK\r\n";
        break;
    case 404:
        status_line = "HTTP/1.1 404 Not Found\r\n";
        break;
    default:
        status_line = "HTTP/1.1 500 Internal Server Error\r\n";
        break;
    }

    // Read the contents of the file into the body
    std::ifstream file(file_path);
    if (file.is_open())
        body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Build the response
    std::string response = status_line + "Content-Length: " + std::to_string(body.size()) + "\r\n" + "Content-Type: text/html\r\n\r\n" + body;

    return response;
}

void http::finalize_response(int fd, std::string data)
{
    write(fd, data.c_str(), data.length());
}

void http::init(const char *message)
{
    headers->init(message);
}

http::http()
{
    headers = new request_headers();
}

http::~http()
{
    delete headers;
}

std::string request_headers::get(const char *key)
{
    return request[key];
}

std::string http::decodeURIComponent(const std::string &encoded_string)
{
    std::string decoded_string;
    char ch;
    std::size_t i;
    int j;
    for (i = 0; i < encoded_string.length(); i++)
    {
        ch = encoded_string[i];
        if (ch == '%')
        {
            sscanf(encoded_string.substr(i + 1, 2).c_str(), "%x", &j);
            ch = static_cast<char>(j);
            i += 2;
        }
        decoded_string += ch;
    }
    return decoded_string;
}