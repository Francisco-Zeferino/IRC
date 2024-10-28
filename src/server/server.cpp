/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaptist <mbaptist@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:53:28 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/28 16:04:50 by mbaptist         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

Server::Server(){}

Server::~Server() {
    std::cout << "Closing server\n";
    handleQuitOnSignal();
}

void Server::handleQuitOnSignal(){
    std::map<int, Client*>::iterator it;
    std::map<int, Client*>::iterator nextIt;
    std::vector<Channel*>::iterator itChannel;
    std::stringstream iss;
    
    for(it = clients.begin(); it != clients.end(); it++){
        nextIt = it;
        nextIt++;
        if(nextIt == clients.end())
            break;
        hQuitCmd(iss, it);
        it = nextIt;
    }
    // delete it->second;
    close(connection);
    close(epollfd);
    close(serverSocket);
    for(itChannel = serverChannels.begin(); itChannel != serverChannels.end(); itChannel++){
        delete *itChannel;
    }
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

void Server::epollState(int epollfd, int socket, uint32_t newEvent){
    static struct epoll_event event;
    event.events = newEvent;
    event.data.fd = socket;
    if(epoll_ctl(epollfd, EPOLL_CTL_MOD, socket, &event)){
        std::cerr << "Error modifying epoll state" << std::endl;
        return ;
    }
}

void Server::handleSignal(int signal){
    (void)signal;
    throw std::runtime_error("Caught signal SIGINT");
}

void Server::setEpoll() {

    signal(SIGINT, handleSignal);
    epollfd = epoll_create1(0);
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
    
    while(1) {
        nfds = epoll_wait(epollfd, clientEvent, 1024, -1);
        if(nfds == -1){
            std::cerr << "Error during epoll wait" << std::endl;
            return;
        }
        for(int i = 0; i < nfds; i++){
            if(clientEvent[i].data.fd == serverSocket)
                setConnection(epollfd);
            else{
                epollState(epollfd, clientEvent[i].data.fd, EPOLLOUT);
                handleClientMessage(clientEvent[i].data.fd);
                epollState(epollfd, clientEvent[i].data.fd, EPOLLIN);
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

Channel* Server::findChannel(const std::string& channelName) {
    for (std::vector<Channel*>::iterator it = serverChannels.begin(); it != serverChannels.end(); ++it) {
        if ((*it)->name == channelName) {
            return *it;
        }
    }
    
    return NULL;
}

Channel* Server::createChannel(const std::string& channelName) {
    std::cout << "Creating new channel: " << channelName << "\n";
    Channel* newChannel = new Channel(channelName);
    
    if (newChannel == NULL) {
        std::cerr << "Error creating channel\n";
        return NULL;
    }
    
    newChannel->setMode("+t");
    newChannel->setMode("+o");
    serverChannels.push_back(newChannel);
    std::cout << "Channel created: " << channelName << " with default modes: +t +o\n";
    return newChannel;
}

void Server::removeChannel(const std::string& channelName) {
    std::vector<Channel*>::iterator it;
    for(it = serverChannels.begin(); it != serverChannels.end(); ++it) {
        if ((*it)->name == channelName) {
            serverChannels.erase(it);
            // delete *it;
            std::cout << "Channel " << channelName << " removed from server.\n";
            return;
        }
    }
    std::cout << "Channel " << channelName << " not found.\n";
}

void Server::setConnection(int epollfd){
    size_t serverAddrSize = sizeof(serverAddr);
    if((connection = accept(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t *)&serverAddrSize)) < 0){
        std::cout << "Error accepting connection" << std::endl;
        exit(1);
    }
    static struct epoll_event connectionEvent;
    connectionEvent.events = EPOLLIN;
    connectionEvent.data.fd = connection;
    fcntl(connection, F_SETFL, O_NONBLOCK);
    epoll_ctl(epollfd, EPOLL_CTL_ADD, connection, &connectionEvent);
    clients.insert(std::pair<int, Client*>(connection, new Client(connection)));
}

void Server::sendMsgServ(const std::string &msg, int clientSocket) const {
    send(clientSocket, msg.c_str(), msg.length(), 0);
}

// Channel* Server::findOrCreateChannel(const std::string& channelName) {
//     std::vector<Channel*>::iterator it;
//     for (it = serverChannels.begin(); it != serverChannels.end(); it++) {
//         if ((*it)->name == channelName) {
//             return *it;
//         }
//     }

//     std::cout << "Channel not found, creating new channel: " << channelName << "\n";
//     Channel* newChannel = new Channel(channelName);
//     if(newChannel == NULL) {
//         std::cout << "Error creating channel\n";
//         return NULL;
//     }
//     newChannel->setMode("+t");
//     newChannel->setMode("+o");
//     serverChannels.push_back(newChannel);
//     std::cout << "Channel created: " << channelName << " with default modes: +t +o\n";
//     return newChannel;
// }