#ifndef BOT_HPP
#define BOT_HPP

#include "client.hpp"

class Bot {
    public:
        Bot();
        Bot(std::string nick, std::string user, int socket);
        ~Bot();

        std::string getUser() const;
        std::string getNick() const;
        int getSocket() const;

    private:
        std::string nick;
        std::string user;
        int socket;
};

#endif

