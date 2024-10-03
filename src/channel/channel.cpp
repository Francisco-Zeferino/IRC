#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

Channel::Channel(std::string name) : name(name), userslimit(0) {}

void Channel::setTopic(const std::string &newTopic) {
    this->topic = newTopic;
}

void Channel::setMode(const std::string& newMode) {
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

bool Channel::hasMode(char modeChar) const {
    return mode.find(modeChar) != std::string::npos;
}

void Channel::addClient(Client* client, bool isOperator) {
    channelMember[client] = isOperator;
}

void Channel::removeClient(Client* client) {
    channelMember.erase(client);
}

void Channel::applyMode(std::stringstream &iss) {
    std::string modeDetails;
    iss >> modeDetails;

    if (modeDetails[0] == '+') {
        setMode(modeDetails);
        std::cout << "Mode " << modeDetails << " applied to channel " << name << "\n";
    } else if (modeDetails[0] == '-') {
        setMode(modeDetails);
        std::cout << "Mode " << modeDetails << " removed from channel " << name << "\n";
    }
}