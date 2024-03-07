#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel {
public:
    Channel(const std::string& name);
    ~Channel();

    void join(Client& client);
    void part(Client& client);

private:
    std::string name;
    std::vector<Client*> members;
    // Add other necessary data structures.
};

#endif // CHANNEL_HPP
