#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

Channel::Channel(std::string name) : name(name), userslimit(0) {}

void Channel::setPassword(const std::string password){
    this->password = password;
}

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
    (void)isOperator;
    clientsInChannel.push_back(client);
}

void Channel::removeClient(Client* client) {
    (void)client;
}

bool Channel::validateUserJoin(const std::string user){
    std::vector<std::string>::iterator it;
    for(it = invUsers.begin(); it != invUsers.end(); it++){
        if(*it == user)
            return true;
    }
    return false;
}

void Channel::applyMode(std::stringstream &iss, const std::string mode) {
    if (mode[0] == '+') {
        setMode(mode);
        if(this->mode == "k")
            iss >> password;
        else if(this->mode == "l")
            iss >> userslimit;
        std::cout << "Mode " << mode << " applied to channel " << name << "\n";
    } else if (mode[0] == '-') {
        setMode(mode);
        std::cout << "Mode " << mode << " removed from channel " << name << "\n";
    }
}