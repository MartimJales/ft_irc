#ifndef SERVER_HPP
#define SERVER_HPP


#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();

    void run();

private:
    int listenSocket;
    std::string password;
    std::vector<Client> clients;
    std::vector<Channel> channels;

    // Add other necessary data structures and methods.
};

#endif // SERVER_HPP
