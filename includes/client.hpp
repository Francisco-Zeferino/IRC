#include <string>
#include <iostream>
#include "server.hpp"

class Client
{
    public:
        Client (int socket);

        std::string getNick() const;
        std::string getUser() const;
        std::string getChannel() const;


        void setNick(const std::string &nickname);
        void setUser(const std::string &username);
        void setNick(const std::string &channelname);
        void sendMsg (const std::string &msg);

    private:
        int _socket;
        std::string _nick;
        std::string _user;
        std::string _channel;

};

//add cmdparser class com handle de:
// nick, user, join, kick, pm;
