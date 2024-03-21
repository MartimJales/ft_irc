#include "Channel.hpp"

Channel::Channel(const std::string& name) : name(name) {
	// Initialize the channel's name.
}

Channel::~Channel() {
	// Remove all members from the channel.
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