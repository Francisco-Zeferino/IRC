#include "channel.hpp"

void Channel::addClient(Client *client) {
    clients.push_back(client);
}

void Channel::removeClient(Client *client) {
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
}

void Channel::setTopic(std::string topic) {
    this->topic = topic;
}