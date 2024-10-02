#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

Channel::Channel(std::string channelName) : name(channelName) {}

void Channel::setTopic(std::string topic) {
    this->topic = topic;
}

bool Channel::hasMode(char modeChar) const {
    return mode.find(modeChar) != std::string::npos;
}

void Channel::setMode(std::string newMode) {
    if (newMode[0] == '+') {
        if (mode.find(newMode[1]) == std::string::npos) {
            mode += newMode[1];
        }
    } else if (newMode[0] == '-') {
        size_t pos = mode.find(newMode[1]);
        if (pos != std::string::npos) {
            mode.erase(pos, 1);
        }
    }
}

void Channel::setName(std::string name) {
    this->name = name;
}


void Channel::addClient(Client* client, bool isOperator) {
    channelMember[client] = isOperator;
}

void Channel::removeClient(Client* client) {
    channelMember.erase(client);
}

