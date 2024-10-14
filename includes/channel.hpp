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

        std::map<Client *, bool> admins;
        std::vector<std::string> invUsers;
        std::vector<std::string> bannedUsers;

        // Chanel management
        void setTopic(const std::string &topic);
        void setMode(const std::string &newMode);
        void setPassword(const std::string password);
        bool hasMode(char mode) const;
        bool validateUserJoin(const std::string user);

        // Modes
        void applyMode(std::stringstream &iss, const std::string mode, Client *requester);
        bool isAdmin(Client *client);
        void aOperatorMode(std::stringstream &iss, bool addOperator, Client* requester);
        // void aOperatorMode(const std::string& targetNick, bool addOperator, Client *requester);
        void aInviteOnlyMode(bool enable);
        void aPasswordMode(std::stringstream &iss, bool enable);
        void aUserLimitMode(std::stringstream &iss, bool enable);

        //Client management
        void addClient(Client* client, bool isOperator = false);
        void removeClient(Client* client);

        void sendMsg(const std::string &msg, int clientSocket);
        void notifyAllInChannel(Channel *channel, std::string message);

};

#endif