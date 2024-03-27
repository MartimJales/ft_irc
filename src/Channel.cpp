#include "Channel.hpp"


// TODO: We need to be carefull with the number of args:
// Example: MODE #teste +k *NOTHING HERE*

Channel::Channel(const std::string& name) : name(name) {
	// Initialize the modes map
	modes["+i"] = &Channel::setInviteOnly;
	modes["-i"] = &Channel::removeInviteOnly;
	modes["+t"] = &Channel::setTopicProtected;
	modes["-t"] = &Channel::removeTopicProtected;
	modes["+k"] = &Channel::setKeyProtected;
	modes["-k"] = &Channel::removeKeyProtected;
	modes["+l"] = &Channel::setUsersLimit;
	modes["-l"] = &Channel::removeUsersLimit;
	modes["+o"]= &Channel::addOperatorPremission;
	modes["-o"]= &Channel::removeOperatorPremission;
	inviteOnly = false;
	topicProtected = false;
	keyProtected = false;
	usersLimit = -1;
}

Channel::~Channel() {
	// Free the modes map
}

std::string Channel::getName() const {
	return name;
}

void Channel::setName(const std::string& name) {
	this->name = name;
}

void Channel::addMember(Client* member) {
	members.push_back(member);
}

void Channel::removeMember(Client* member) {
	std::vector<Client*>::iterator it;
	for (it = members.begin(); it != members.end(); ++it) {
		if (*it == member) {
			members.erase(it);
			break;
		}
	}
}

void Channel::addOperator(Client* op) {
	operators.push_back(op);
}

void Channel::removeOperator(Client* op) {
	std::vector<Client*>::iterator it;
	for (it = operators.begin(); it != operators.end(); ++it) {
		if (*it == op) {
			operators.erase(it);
			break;
		}
	}
}

void Channel::listMembers() {
	std::cout << "List of operators of channel " << name << ":" << std::endl;
	std::vector<Client*>::iterator it;
	for (it = operators.begin(); it != operators.end(); ++it) {
		std::cout << (*it)->getNickName() << std::endl;
	}
	std::cout << "List of members of channel " << name << ":" << std::endl;
	for (it = members.begin(); it != members.end(); ++it) {
		std::cout << (*it)->getNickName() << std::endl;
	}
}

void Channel::setTopic(const std::string& topic) {
	this->topic = topic;
}

std::string Channel::getTopic() const {
	return topic;
}

bool Channel::isOperator(Client* client) {
	std::vector<Client*>::iterator it;
	for (it = operators.begin(); it != operators.end(); ++it) {
		if (*it == client) {
			return true;
		}
	}
	return false;
}

void Channel::addOperatorPremission(char *tokens, Client *client)
{
	tokens = std::strtok(NULL, " \n\r");
	//check that client is in the members vector and give operator to said client
	std::vector<Client*>::iterator it;
	for (it = members.begin(); it != members.end(); ++it) {
		if ((*it)->getNickName() == tokens) {
			operators.push_back(*it);
			break;
		}
	}
	std::cout << "Client " << tokens << " is now an operator of channel " << name << std::endl;
	(void)client;
}

void Channel::removeOperatorPremission(char *tokens, Client *client)
{
	tokens = std::strtok(NULL, " \n\r");
	//remove operator permission to the client
	std::vector<Client*>::iterator it;
	for (it = operators.begin(); it != operators.end(); ++it) {
		if ((*it)->getNickName() == tokens) {
			operators.erase(it);
			break;
		}
	}
	std::cout << "Client " << tokens << " is no longer an operator of channel " << name << std::endl;
	(void)client;
}

void Channel::handleModes(char *tokens, Client *client) {
	tokens = std::strtok(NULL, " \n\r");
	// Compare the tokens and map values
	if (modes.find(tokens) == modes.end())
	{
		std::cout << "Modes " << tokens << " does not exist." << std::endl;
		tokens = std::strtok(NULL, " \n\r");
		return;
	}
	// Run the command
	(this->*modes[tokens])(tokens, client);
}

void Channel::setInviteOnly(char *tokens, Client *client) {
	inviteOnly = true;
	std::cout << "Channel " << name << " is now invite only." << std::endl;
	(void)tokens;
	(void)client;
}

void Channel::removeInviteOnly(char *tokens, Client *client) {
	inviteOnly = false;
	std::cout << "Channel " << name << " is no longer invite only." << std::endl;
	(void)tokens;
	(void)client;
}

void Channel::setTopicProtected(char *tokens, Client *client) {
	topicProtected = true;
	std::cout << "Channel " << name << " is now topic protected." << std::endl;
	(void)tokens;
	(void)client;
}

void Channel::removeTopicProtected(char *tokens, Client *client) {
	topicProtected = false;
	std::cout << "Channel " << name << " is no longer topic protected." << std::endl;
	(void)tokens;
	(void)client;
}

void Channel::setKeyProtected(char *tokens, Client *client) {
	keyProtected = true;
	std::cout << "Channel " << name << " is now key protected." << std::endl;
	tokens = std::strtok(NULL, " \n\r");
	key = tokens;
	std::cout << "Key: " << key << std::endl;
	(void)client;
}

void Channel::removeKeyProtected(char *tokens, Client *client) {
	keyProtected = false;
	std::cout << "Channel " << name << " is no longer key protected." << std::endl;
	(void)tokens;
	(void)client;
}

void Channel::setUsersLimit(char *tokens, Client *client) {
	tokens = std::strtok(NULL, " \n\r");
	usersLimit = std::atoi(tokens);
	if (usersLimit < 1) {
		std::cout << "Invalid limit of users." << std::endl;
		return;
	}
	std::cout << "Channel " << name << " now has a limit of " << usersLimit << " users." << std::endl;
		(void)tokens;
	(void)client;

}

void Channel::removeUsersLimit(char *tokens, Client *client) {
	usersLimit = -1;
	std::cout << "Channel " << name << " no longer has a limit of users." << std::endl;
	(void)tokens;
	(void)client;
}

// Implement the operator << to print the channel name, the topic, number of members, and modes
// EXAMPLE: #<channelname>{<topic>}[<number of members>](<modes>)
std::ostream& operator<<(std::ostream& os, const Channel& channel) {
	os << channel.getName() << "{" << channel.getTopic() << "}";
	if (channel.inviteOnly){
		// std::cout << "Quero adicionar o +i" << std::endl;
		os << "(+i)";
	}
	if (channel.topicProtected)
		os << "(+t)";
	if (channel.keyProtected)
		os << "(+k)";
	if (channel.usersLimit != (long unsigned int)-1)
		os << "(+l)";

	// Show nicks of operators
	std::vector<Client*>::const_iterator it;
	for (it = channel.operators.begin(); it != channel.operators.end(); ++it) {
		os << (*it)->getNickName() << " ";
	}
	return os;
}

void Channel::join(Client *client){
	//check if the client is already in the channel
	std::vector<Client*>::iterator it;
	for (it = members.begin(); it != members.end(); ++it) {
		if (*it == client) {
			std::cout << "Client " << client->getNickName() << " is already in channel " << name << std::endl;
			return;
		}
	}
	//check if the channel is invite only
	if (inviteOnly) {
		std::cout << "Channel " << name << " is invite only." << std::endl;
		return;
	}
	//check if the channel is full
	if (members.size() >= usersLimit && usersLimit != (long unsigned int)-1) {
		std::cout << "Channel " << name << " is full." << std::endl;
		return;
	}
	//check if the channel is key protected
	if (keyProtected) {
		char *tokens = std::strtok(NULL, " \n\r");
		if (tokens == NULL || key != tokens) {
			std::cout << "Channel " << name << " is key protected." << std::endl;
			return;
		}
	}
	//add the client to the channel
	members.push_back(client);
	if (members.size() == 1) {
		operators.push_back(client);
	}
	std::cout << "Client " << client->getNickName() << " joined channel " << name << std::endl;
}
