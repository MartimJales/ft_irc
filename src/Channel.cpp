#include "Channel.hpp"

Channel::Channel(const std::string& name) : name(name) {
	// Initialize the channel's name.
}

Channel::~Channel() {
	// Remove all members from the channel.
}

void Channel::join(Client& client) {
	(void)client;
	// Add the client to the channel's list of members.
}

void Channel::part(Client& client) {
	(void)client;
	// Remove the client from the channel's list of members.
}

