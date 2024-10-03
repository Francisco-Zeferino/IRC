#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <vector>
#include <map>
#include <poll.h>
#include <fcntl.h> 
#include <algorithm>
#include <sstream>
#include "server.hpp"
#include "channel.hpp"

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

class Server;
class Channel;

class Client
{
    public:
        Client(int socket);

        // Pointer to the client is conected
        Server *server;

        // Getters
        int getSocket() const;
        std::string getNick() const;
        std::string getUser() const;
        std::string getChannel() const;

        // Setters
        void setNick(const std::string &nickname);
        void setUser(const std::string &username);
        void setChannel(const std::string &channelname);

        // Parsing & cmds
        void parseMessage(const std::string &message, std::map<int, Client*>::iterator it);
        void hNickCmd(std::stringstream &iss);
        void hUserCmd(std::stringstream &iss);
        void hJoinCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hPartCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hPrivMsgCmd(std::stringstream &iss);
        void hKickCmd(std::stringstream &iss);
        void hInviteCmd(std::stringstream &iss);
        void hTopicCmd(std::stringstream &iss);
        void hModeCmd(std::stringstream &iss);

        void sendMsg(const std::string &msg, int clientSocket) const;

    private:
        int _socket;
        std::string _nick;
        std::string _user;
        std::string _channel;

};

#endif
