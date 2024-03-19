#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

class Client {
public:
    Client(int socket);
    ~Client();

    void processInput();
	int getClientSocket();

private:
    int clientSocket;
    std::string nickname;
    std::string username;

    // Add other necessary data structures and methods.
};

#endif // CLIENT_HPP
