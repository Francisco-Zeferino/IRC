#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"
#include <vector>
#include <map>
#include <string>

class Channel {
    public :
        std::vector<Client *> clients;
        std::map<Client *, bool> channel;
        std::string name;
        std::string topic;
        std::string mode;
        void addClient(Client *client);
        void removeClient(Client *client);
        void setTopic(std::string topic);
};

#endif