#include "bot.hpp"
#include "server.hpp"

Bot::Bot() {}

Bot::Bot(std::string nick, std::string user, int socket) : nick(nick), user(user), socket(socket) {}

std::string Bot::getUser() const {
    return user;
}

std::string Bot::getNick() const {
    return nick;
}

int Bot::getSocket() const {
    return socket;
}

int Server::setBot(){
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

Bot *Server::createBot(){
    int botSocket = setBot();
    return new Bot("rovy", "rover", botSocket);
}


