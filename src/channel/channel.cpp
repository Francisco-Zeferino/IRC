#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

Channel::Channel(std::string name) : name(name), userslimit(0) {}

void Channel::setPassword(const std::string password){
    this->password = password;
}

std::string Channel::getTopic() const {
    return topic;
}

void Channel::setTopic(const std::string &newTopic, Client* requester) {
    if (hasMode('t') && !isAdmin(requester)) {
        sendMsg(ERR_CHANOPRIVSNEEDED(requester->getNick(), name), requester->getSocket());
        return;
    }

    topic = newTopic;
    std::string message = ":" + requester->getNick() + "!" + requester->getUser() + "@localhost TOPIC " + name + " :\00308" + newTopic + "\r\n";
    notifyAllInChannel(this, message);
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

void Channel::removeClient(Client* client) {
    admins.erase(client);

    std::vector<std::string>::iterator invIt = std::find(invUsers.begin(), invUsers.end(), client->getNick());
    if (invIt != invUsers.end()) {
        invUsers.erase(invIt);
    }
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
    size_t i = send(clientSocket, msg.c_str(), msg.length(), 0);
    if (i != msg.size()) {
        std::cerr << "Error sending message to client" << std::endl;
    }
}

void Channel::notifyAllInChannel(Channel *channel, std::string message){
    std::map<Client *, bool>::iterator it;
    
    it = channel->admins.begin();
    while(it != channel->admins.end()) {
        sendMsg(message,it->first->getSocket());
        it++;
    }
}

void Channel::notifyAllClients(Client *client, std::string message){
    std::map<Client *, bool>::iterator it;
    
    it = admins.begin();
    while(it != admins.end()) {
        if(client != it->first)
            sendMsg(message,it->first->getSocket());
        it++;
    }
}

