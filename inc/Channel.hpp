#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel {
public:
    Channel(const std::string& name);
    ~Channel();

    // Getters
    std::string getName() const;
    std::string getTopic() const;

    //Setters
    void setName(const std::string& name);
    void setTopic(const std::string& topic);

    //Methods
    void addMember(Client* member);
    void removeMember(Client* member);
    void addOperator(Client* op);
    void removeOperator(Client* op);
    void listMembers();
    bool isOperator(Client* client);

private:
    std::string name;
    std::string topic;
    std::vector<Client*> members;
    std::vector<Client*> banned;
    std::vector<Client*> operators;
};

#endif // CHANNEL_HPP
