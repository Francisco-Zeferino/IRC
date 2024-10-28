#ifndef BOT_HPP
#define BOT_HPP

#include "client.hpp"
// #include "server.hpp"
// #include "channel.hpp"
// #include "irc.hpp"

// class Server;
// class Channel;
// class Client;

class Bot : public Client {
    public:
        Bot(int socket);
        void banana();

    private:
    // void valitadeBotMsg(const std::string &message);

    // void greetBotMsg(Channel *channel);
    // void timeBotMsg(Channel* channel);
    // void sendBotMsg(Channel* channel, const std::string& message);
        
        // int botSocket;
        // std::string _botnick;
        // std::string _botuser;
};


#endif 



// class Bot : public Client {
//     public:
//         Bot(int socket, const std::string &nick, const std::string &user);
//         void valitadeBotMsg(const std::string &message, Channel *channel);

//     private:
//         void greetBotMsg(Channel *channel);
//         void timeBotMsg(Channel *channel);
//         void sendBotMsg(Channel *channel, const std::string &message);
// };