#include "iostream"

class Bot {
    public:
        Bot();
        Bot(std::string nick, std::string user, int socket);
        std::string getUser() const;
        std::string getNick() const;
        int getSocket() const;
    private:
        std::string channel;
        std::string nick;
        std::string user;
        int socket;
};