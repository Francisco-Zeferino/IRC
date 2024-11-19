#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "server.hpp"
// #include "channel.hpp"

#define MAX_CLIENTS 100
#define BUFFER_SIZE 100 //porque alteraste?

class Channel;

class Client
{
    public:
        Client();
        Client(int socket);

        std::vector<Channel *> clientChannels;
        std::map<Client *, std::string> filePool;
        std::string messageBuffer;
        bool isAuthenticated;
        bool passwordAuthenticated;
        // Getters
        int getSocket() const;
        std::string getNick() const;
        std::string getUser() const;

        // Setters
        void setNick(const std::string &nickname);
        void setUser(const std::string &username);

    private:

    protected:
        int _socket;
        std::string _nick;
        std::string _user;
};

#endif
