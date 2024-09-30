#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <vector>
#include <map>
#include <poll.h>
#include <fcntl.h> 
#include <algorithm>
#include <sstream>
#include "server.hpp"

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

class Client
{
    public:
        Client(int socket);

        int getSocket() const;
        std::string getNick() const;
        std::string getUser() const;
        std::string getChannel() const;

        void setNick(const std::string &nickname);
        void setUser(const std::string &username);
        void setChannel(const std::string &channelname);

        void sendMsg(const std::string &msg) const;


        // Parsing
        void parseMessage(const std::string &message);
        void hNickCmd(std::istringstream &iss);
        void hUserCmd(std::istringstream &iss);
        void hJoinCmd(std::istringstream &iss);
        void hPrivMsgCmd(std::istringstream &iss);
        void hKickCmd(std::istringstream &iss);
        void hInviteCmd(std::istringstream &iss);
        void hTopicCmd(std::istringstream &iss);
        void hModeCmd(std::istringstream &iss);

    private:
        int _socket;
        std::string _nick;
        std::string _user;
        std::string _channel;

};

#endif
