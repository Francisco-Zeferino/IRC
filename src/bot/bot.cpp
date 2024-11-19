#include "bot.hpp"
#include "server.hpp"
#include "channel.hpp"

Bot::Bot() {}

Bot::Bot(std::string nick, std::string user, int socket)
    : nick(nick), user(user), socket(socket) {}

Bot::~Bot() {
    close(socket);
};

std::string Bot::getUser() const {
    return user;
}

std::string Bot::getNick() const {
    return nick;
}

int Bot::getSocket() const {
    return socket;
}

int Server::setBot() {
    int botSocket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(botSocket, F_SETFL, O_NONBLOCK);
    if (botSocket < 0) {
        std::cerr << "Error creating bot socket\n";
        throw std::runtime_error("Error creating bot socket");
    }
    struct epoll_event botEvent;
    botEvent.events = EPOLLIN;
    botEvent.data.fd = botSocket;
    int await = epoll_ctl(epollfd, EPOLL_CTL_ADD, botSocket, &botEvent);
    if(await < 0){
        std::cerr << "Error adding bot socket to epoll\n";
        throw std::runtime_error("Error adding bot socket to epoll");
    }
    return botSocket;
}

Bot Server::createBot() {
    int botSocket = setBot();
    return Bot("BoTony", "marvin", botSocket);
}

void Server::hBotCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string command, channelName;
    iss >> command >> channelName;
    std::cout << " STRING: "<< command << " " << channelName << " " << "\n";
    if (command == "hangman") {
        if (channelName.empty()) {
            sendMsgServ("Usage: /BOT hangman <start|guess|solve> <channel> [word|letter]\n", it->first);
            return;
        }
        aBotHangman(it, channelName, iss);
    } else if (command == "join") {
        aBotJoin(it, channelName);
    } else if (command == "help") {
        aBotHelp(it, channelName);
    } else if (command == "leave") {
        aBotLeave(it, channelName);
    } else if (command == "hello" || command == "ola") {
        aBotHello(it, channelName);
    } else if (command == "time") {
        aBotTime(it, channelName);
    } else {
        sendMsgServ("Unknown BOT command.\n", it->first);
    }
}

void Server::aBotJoin(std::map<int, Client*>::iterator it, const std::string &channelName) {
    Channel* ch = findChannel(channelName);
    if (!ch) {
        sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    std::map<Client*, bool>::iterator user = ch->admins.find(it->second);
    if (user == ch->admins.end() || !user->second) {
        sendMsgServ(ERR_CHANOPRIVSNEEDED(it->second->getNick(), channelName), it->first);
        return;
    }

    for (std::vector<Bot>::iterator botIt = ch->bots.begin(); botIt != ch->bots.end(); ++botIt) {
        if ((botIt)->getNick() == "BoTony") {
            sendMsgServ("Bot is already in the channel.\n", it->first);
            return;
        }
    }

    Bot bot = createBot();
    ch->bots.push_back(bot);
    std::string message = ":+" + bot.getNick() + "!" + bot.getUser() + "@localhost JOIN " + ch->name + "\r\n";
    ch->notifyAllInChannel(ch, message);
}

void Server::aBotHelp(std::map<int, Client*>::iterator it, const std::string &channelName) {
    Channel* ch = findChannel(channelName);
    if (!ch) {
        sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    if (!ch->hasBot("BoTony")) {
        sendMsgServ("BoTony is not in the channel.\n", it->first);
        return;
    }

    std::string message = "Commands available: join, ola, time, leave, gg.\r\n";
    ch->notifyAllInChannel(ch, RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, message));
}

void Server::aBotLeave(std::map<int, Client*>::iterator it, const std::string &channelName) {
    Channel* ch = findChannel(channelName);
    if (!ch) {
        sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    std::vector<Bot >::iterator botIt;
    for(botIt = ch->bots.begin(); botIt != ch->bots.end(); botIt++){
        ch->notifyAllInChannel(ch ,RPL_PART(user_info((botIt)->getNick(), (botIt)->getUser()), channelName));
        ch->bots.erase(botIt);
        break;
    }
}


void Server::aBotTime(std::map<int, Client*>::iterator it, const std::string &channelName) {
    Channel* ch = findChannel(channelName);
    if (!ch) {
        sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    if (!ch->hasBot("BoTony")) {
        sendMsgServ("BoTony is not in the channel.\n", it->first);
        return;
    }

    std::time_t currentTime = std::time(0);
    std::string timeStr = std::ctime(&currentTime);
    timeStr.erase(timeStr.find_last_not_of(" \n\r\t") + 1);

    std::string timeResponse = "Current time is: " + timeStr + " ";
    ch->notifyAllInChannel(ch, RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, timeResponse));
}


void Server::aBotHello(std::map<int, Client*>::iterator it, const std::string &channelName) {
    Channel* ch = findChannel(channelName);
    if (!ch) {
        sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    if (!ch->hasBot("BoTony")) {
        sendMsgServ("BoTony is not in the channel.\n", it->first);
        return;
    }

    std::string helloResponse = "YOOOOOOOOOOOOO, tudo fixe " + it->second->getNick() + "?";
    ch->notifyAllInChannel(ch, RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, helloResponse));
}

bool Channel::hasBot(const std::string& botNick) const {
    for (std::vector<Bot>::const_iterator botIt = bots.begin(); botIt != bots.end(); ++botIt) {
        if (botIt->getNick() == botNick) {
            return true;
        }
    }
    return false;
}