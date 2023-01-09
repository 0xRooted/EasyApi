#include "server/server.hpp"
#include "utils/utils.hpp"

Server *server = nullptr;

auto main() -> int
{
    server = new Server("0.0.0.0", 8080);

    if (!server->init())
    {
        std::cout
            << "[SERVER SETUP FAILED]"
            << std::endl;
        return EXIT_SUCCESS;
    }

    server->add_endpoint
    (
        "/", // path
        [](user *u)
        {
            if (u->request->headers->get("method") == "GET")
            {
                u->request->finalize_response(u->fd, u->request->craft_response_html(200, "<!DOCTYPE html><html><head><title>Success</title></head><body><h1>SUCCESS</h1></body></html>"));
                return;
            }
            else if (u->request->headers->get("method") == "POST")
            {
                u->request->finalize_response(u->fd, u->request->craft_response_html(400, "<!DOCTYPE html><html><head><title>Unauthorized</title></head><body><h1>Unauthorized Request</h1><p>The requested resource was not authorized on this server.</p></body></html>"));
                return;
            }
        }
    );
    
    std::thread handler(&Server::loop, server);
    std::cout
        << "[SERVER SETUP SUCCESS && STARTED]\n"
        << std::endl;
    handler.join();

    return EXIT_SUCCESS;
}