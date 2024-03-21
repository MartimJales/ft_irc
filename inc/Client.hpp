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
	// Setters
	void setNickname(std::string nickname);
    void setUsername(std::string username);
    void setCurrentChannel(std::string channel);

    //Getters
    std::string getNickName(void);
    std::string getUsername(void);
    std::string getCurrentChannel(void);

private:
    int clientSocket;
    std::string nickname;
    std::string username;
    std::string currentChannel;

    // Add other necessary data structures and methods.
};

#endif // CLIENT_HPP
