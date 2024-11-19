#include "bot.hpp"
#include "server.hpp"
#include "channel.hpp"

Bot::Bot() {}

Bot::~Bot(){
    close(socket);
}

Bot::Bot(std::string nick, std::string user, int socket)
    : nick(nick), user(user), socket(socket) {}

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



// void Server::hBotCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
//     std::string command, channel;
//     iss >> command >> channel;

//     Channel* ch = findChannel(channel);

//     if(command == "join") {
//         if (!ch) {
//             sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channel), it->first);
//             return;
//         }
        
//         std::map<Client *, bool>::iterator user = ch->admins.find(it->second);
//         if(user->second == false){
//             sendMsgServ(ERR_CHANOPRIVSNEEDED(it->second->getNick(), channel), it->first);
//             return;
//         }

//         std::vector<Bot*>::iterator botIt;
//         for (botIt = ch->bots.begin(); botIt != ch->bots.end(); ++botIt) {
//             if ((*botIt)->getNick() == "BoTony") {
//                 sendMsgServ("Bot is already in the channel.\n", it->first);
//                 return;
//             }
//         }

//         Bot *bot = createBot();
//         ch->bots.push_back(bot);
//         std::string message = ":+" + bot->getNick() + "!" + bot->getUser() + "@localhost JOIN " + ch->name + "\r\n";
//         ch->notifyAllInChannel(ch, message);
//     }



//     else if(command == "help") {
//         std::string message = " : Commands available: join, ola, time, leave, gg.\r\n";
//         sendMsgServ(RPL_PRIVMSG(user_info(it->second->getNick(), it->second->getUser()), channel, message), it->first);
//     }

//     else if(command == "leave") {
//         std::vector<Bot *>::iterator botIt;
//         if (!ch) {
//             sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channel), it->first);
//             return;
//         }
//         for(botIt = ch->bots.begin(); botIt != ch->bots.end(); botIt++){
//             ch->notifyAllInChannel(ch ,RPL_PART(user_info((*botIt)->getNick(), (*botIt)->getUser()), ch->name));
//             ch->bots.erase(botIt);
//             break;
//         }
//     }
// }



void Server::hBotCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string command, arg2, arg3;
    iss >> command >> arg2 >> arg3;
    if(isClientAuthenticated(it->second) == false){
        std::cout << "Not authenticated to server." << std::endl;
        return ;
    }
    if (command == "join") {
        aBotJoin(it, arg2);
    }
    else if (command == "help") {
        aBotHelp(it, arg2);
    }
    else if (command == "leave") {
        aBotLeave(it, arg2);
    }
    else if (command == "hello" || command == "ola") {
        aBotHello(it, arg2);
    }
    else if (command == "time") {
        aBotTime(it, arg2);
    }
    else {
        sendMsgServ("Unknown command\n", it->first);
    }
}

// Function to handle 'join' command
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
    std::string message = " : Commands available: join, ola, time, leave, gg.\r\n";
    sendMsgServ(RPL_PRIVMSG(user_info(it->second->getNick(), it->second->getUser()), channelName, message), it->first);
}

void Server::aBotLeave(std::map<int, Client*>::iterator it, const std::string &channelName) {
    Channel* ch = findChannel(channelName);
    if (!ch) {
        sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    std::vector<Bot >::iterator botIt;
    for(botIt = ch->bots.begin(); botIt != ch->bots.end(); botIt++){
        ch->notifyAllInChannel(ch ,RPL_PART(user_info((botIt)->getNick(), (botIt)->getUser()), ch->name));
        ch->bots.erase(botIt);
        break;
    }
}


void Server::aBotHello(std::map<int, Client*>::iterator it, const std::string &channelName) {
    // Channel* ch = findChannel(channelName);
    // if (!ch) {
    //     sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
    //     return;
    // }

    std::string helloResponse = " YOOOOOOOOOOOOO, " + it->second->getNick() + "!";
    sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, helloResponse), it->first);
}

// Function to handle 'time' command
void Server::aBotTime(std::map<int, Client*>::iterator it, const std::string &channelName) {
    // Channel* ch = findChannel(channelName);
    // if (!ch) {
    //     sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
    //     return;
    // }


    std::time_t currentTime = std::time(0);
    std::string timeStr = std::ctime(&currentTime);
    timeStr.erase(timeStr.find_last_not_of(" \n\r\t") + 1);

    std::string timeResponse = "Current time is: " + timeStr + " ";
    sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, timeResponse), it->first);
}
