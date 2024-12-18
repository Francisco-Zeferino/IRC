#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"
#include "bot.hpp"

class Client;
class Bot;

class Channel {
    public :
        Channel(std::string name);

        std::string name;
        std::string topic;      //+t
        std::string mode;       //+i
        std::string password;   //+k
        size_t      userslimit; //+l
        
        std::map<Client *, bool> admins;
        std::vector<Bot > bots;
        std::vector<std::string> invUsers;
        Bot *bot;

        // Chanel management
        void setTopic(const std::string &newTopic, Client* requester);
        std::string getTopic() const;
        void setMode(const std::string &newMode);
        void setPassword(const std::string password);
        bool hasMode(char mode) const;
        bool validateUserJoin(const std::string user);

        // Modes
        void applyMode(std::stringstream &iss, const std::string mode, Client *requester);
        bool isAdmin(Client *client);
        void aOperatorMode(std::stringstream &iss, bool addOperator, Client *requester);
        void aInviteOnlyMode(bool enable);
        void aPasswordMode(std::stringstream &iss, bool enable, Client* requester);
        void aUserLimitMode(std::stringstream &iss, bool enable);
        void aTopicMode(bool enable);

        //Client management
        void addClient(Client *client, bool isOperator = false);
        void removeClient(Client *client);

        // Comunication
        void sendMsg(const std::string &msg, int clientSocket);
        void notifyAllInChannel(Channel *channel, std::string message);
        void notifyAllClients(Client *client, std::string message);

        // Bot
        bool hasBot(const std::string& botNick) const;
        // hangman values
        bool hangmanActive;
        std::string hangmanWord;
        std::string hangmanProgress;
        std::vector<char> hangmanGuesses;
        int hangmanLives;
        // Hangman functions
        bool startHangman(const std::string& word);
        bool guessLetter(char letter, std::string& resultMsg);
        bool solveWord(const std::string& guess, std::string& resultMsg);
        void resetHangman();

};

#endif