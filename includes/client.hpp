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

        std::vector<Channel *> clientChannels;

        // Getters
        int getSocket() const;
        std::string getNick() const;
        std::string getUser() const;
        std::string getChannel() const;

        // Setters
        void setNick(const std::string &nickname);
        void setUser(const std::string &username);
        void setChannel(const std::string &channelname);

    private:
        int _socket;
        std::string _nick;
        std::string _user;
        std::string _channel;

};

#endif
