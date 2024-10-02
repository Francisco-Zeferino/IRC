#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"
#include <vector>
#include <map>
#include <string>

class Server;

class Channel {
    public :
        Channel(std::string name);

        std::string name;
        std::string topic;
        std::string mode;       //+i
        std::string password;   //+k
        size_t userslimit;      //+l

        std::map<Client *, bool> channelMember;

        void setTopic(std::string topic);
        void setName(std::string name);
        void setMode(std::string newMode);
        bool hasMode(char mode) const;
        void hModeCmd(Server* server, const std::string& channelName);

        void addClient(Client* client, bool isOperator = false);
        void removeClient(Client* client);
        
};

#endif