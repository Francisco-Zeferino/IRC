#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"

class Client;

class Channel {
    public :
        Channel(std::string name);

        std::string name;
        std::string topic;
        std::string mode;       //+i
        std::string password;   //+k
        size_t userslimit;      //+l

        std::vector<Client *> clientsInChannel;
        std::vector<std::string> invUsers;

        // Chanel management
        void setTopic(const std::string& topic);
        void setMode(const std::string& newMode);
        bool hasMode(char mode) const;
        bool validateUserJoin(const std::string user);

        void applyMode(const std::string mode);

        //Client management
        void addClient(Client* client, bool isOperator = false);
        void removeClient(Client* client);

};

#endif