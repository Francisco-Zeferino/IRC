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

void Channel::setMode(const std::string &newMode) {
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

void Channel::addClient(Client *client, bool isOperator) {
    admins.insert(std::make_pair(client, isOperator));
}

void Channel::removeClient(Client *client) {
    admins.erase(client); // erase
}

bool Channel::isAdmin(Client* client) {
    std::map<Client *, bool>::iterator it;

    it = admins.find(client);
    if(it->second == true) {
        return true;
    }
    else
        return false;
}

bool Channel::validateUserJoin(const std::string user) {
    std::vector<std::string>::iterator it;
    for(it = invUsers.begin(); it != invUsers.end(); it++){
        if(*it == user)
            return true;
    }
    return false;
}

void Channel::sendMsg(const std::string &msg, int clientSocket) {
    send(clientSocket, msg.c_str(), msg.length(), 0);
}

void Channel::notifyAllInChannel(Channel *channel, std::string message){
    std::map<Client *, bool>::iterator it;
    it = channel->admins.begin();
    while(it != channel->admins.end()) {
        sendMsg(message,it->first->getSocket());
        it++;
    }
}