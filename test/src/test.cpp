#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
	error(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_port> <password>\n";
        return 1;
    }

    int clientSocket, port;
    struct sockaddr_in serverAddress;
    char buffer[256];

    // Parse command line arguments
    port = atoi(argv[1]);
    const char *password = argv[2];

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
        error("Error opening socket");

    // Initialize server address structure
    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
        error("Error connecting to server");

    // Send the password to the server
    int n = write(clientSocket, password, strlen(password));
    if (n < 0)
        error("Error writing to socket");

    // Infinite loop to receive input from the user and send it to the server
    while (true) {
        std::cout << "Enter a message (type 'exit' to quit): ";
        std::cin.getline(buffer, sizeof(buffer));

        // Check if the user wants to exit
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Send the user input to the server
        n = write(clientSocket, buffer, strlen(buffer));
        if (n < 0)
            error("Error writing to socket");
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
