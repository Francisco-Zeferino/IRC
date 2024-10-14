/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:53:28 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/14 15:30:14 by ffilipe-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

Server::Server(){}

Server::~Server() {
    // for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
    //     delete it->second;
    // }
}

void Server::setupServer(char *port, char *password){
    _port = port;
    this->password = password;
    createSocket();
    bindSocket();
    listenSocket();
    setEpoll();
}

void Server::createSocket(){
    int opt = 1;
    std::cout << "Binding socket to port " << _port << std::endl;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(_port));
    serverAddr.sin_addr.s_addr = INADDR_ANY;
}

void Server::bindSocket(){
    if(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        std::cerr << "Error binding socket" << std::endl;
        exit(1);
    }
}

void Server::listenSocket(){
    if(listen(serverSocket, MAX_CLIENTS) < 0){
        std::cerr << "Error listening on socket" << std::endl;
        exit(1);
    }
}

void Server::setEpoll(){
    int epollfd = epoll_create1(0);
    int nfds;
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = serverSocket;
    if(epollfd == -1){
        std::cerr << "Error creating epoll" << std::endl;
        return ;
    }

    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &event)){
        std::cout << "Failed to add file descriptor to epoll" << std::endl;
        return ;
    }
    struct epoll_event clientEvent[1024];
    while(1){
        nfds = epoll_wait(epollfd, clientEvent, 1024, -1);
        if(nfds == -1){
            std::cerr << "Error during epoll wait" << std::endl;
            return;
        }
        for(int i = 0; i < nfds; i++){
            if(clientEvent[i].events & EPOLLIN){
                if(clientEvent[i].data.fd == serverSocket)
                    setConnection(epollfd);
                else
                    handleClientMessage(clientEvent[i].data.fd);

            }
        }
    }
}

Client* Server::getClient(const std::string user){
    std::map<int, Client *>::iterator it;
    for(it = clients.begin(); it != clients.end(); it++){
        if(it->second->getNick() == user)
            return it->second;
    }
    return NULL;
}

void Server::notifyAllInChannel(Channel *channel, std::string message){
    std::map<Client *, bool>::iterator it;
    it = channel->admins.begin();
    while(it != channel->admins.end()){
        sendMsg(message,it->first->getSocket());
        it++;
    }
}

bool Server::validateChannelModes(std::stringstream &iss, std::map<int, Client*>::iterator it, Channel *channel){
    std::string message = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost JOIN " + channel->name + "\r\n";
    if(channel->hasMode('i')){
        if(channel->validateUserJoin(it->second->getNick())){
            std::cout << "Client " << it->second->getSocket() << " joined channel " << channel->name << "\n";
            channel->addClient(it->second);
            notifyAllInChannel(channel, message);
            return true;
        }
        else{
            std::cout << "Can't join Channel, not invited!" << std::endl;
            return true;
        }
    }
    else if(channel->hasMode('k')){
        std::string password;
        iss >> password;
        std::cout << channel->password << std::endl;
        if(channel->password == password){
            std::cout << "Client " << it->second->getSocket() << " joined channel " << channel->name << "\n";
            channel->addClient(it->second);
            sendMsg(message, it->first);
            return true;
        }
        else{
            std::cout << "Wrong password!" << std::endl;
            return true;
        }
    }
    else if(channel->hasMode('l')){
        if(channel->admins.size() >= channel->userslimit){
            std::string message = ERR_CHANNELISFULL(it->second->getNick(), channel->name);
            sendMsg(message,it->first);
            return true;
        }
        else{
            std::cout << "Client " << it->second->getSocket() << " joined channel " << channel->name << "\n";
            channel->addClient(it->second);
            sendMsg(message, it->first);
            return true;
        }
    }
    return false;
}

Channel* Server::findOrCreateChannel(const std::string& channelName) {
    std::vector<Channel*>::iterator it;
    for(it = serverChannels.begin(); it != serverChannels.end(); it++){
        if((*it)->name == channelName)
            break;
    }
    if (it == serverChannels.end()) {
        std::cout << "Channel not found, creating new channel: " << channelName << "\n";
        Channel* newChannel = new Channel(channelName);
        serverChannels.push_back(newChannel);
        std::cout << "Channel created: " << channelName << "\n";
        return newChannel;
    }
    return *it;
}

// void Server::removeChannel(const std::string& channelName) {
//     std::map<std::string, Channel*>::iterator it = channels.find(channelName);
//     if (it != channels.end()) {
//         delete it->second;
//         channels.erase(it);
//         std::cout << "Channel " << channelName << " removed from server.\n";
//     } else {
//         std::cout << "Channel " << channelName << " not found.\n";
//     }
// }

void Server::setConnection(int epollfd){
    size_t serverAddrSize = sizeof(serverAddr);
    if((connection = accept(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t *)&serverAddrSize)) < 0){
        std::cout << "Error accepting connection" << std::endl;
        exit(1);
    }
    static struct epoll_event connectionEvent;
    connectionEvent.events = EPOLLIN;
    connectionEvent.data.fd = connection;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, connection, &connectionEvent);
    clients.insert(std::pair<int, Client*>(connection, new Client(connection)));
}

void Server::sendMsg(const std::string &msg, int clientSocket) const {
    send(clientSocket, msg.c_str(), msg.length(), 0);
}