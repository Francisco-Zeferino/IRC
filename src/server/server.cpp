/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:53:28 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/04 13:16:48 by ffilipe-         ###   ########.fr       */
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

void Server::setupServer(char *port){
    _port = port;
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
        std::cout << "Error binding socket" << std::endl;
        exit(1);
    }
}

void Server::listenSocket(){
    if(listen(serverSocket, MAX_CLIENTS) < 0){
        std::cout << "Error listening on socket" << std::endl;
        exit(1);
    }
}

void Server::setEpoll(){
    int epollfd = epoll_create1(0);
    int nfds;
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = serverSocket;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &event);
    struct epoll_event clientEvent[1024];
    while(1){
        nfds = epoll_wait(epollfd, clientEvent, 1024, -1);
        for(int i = 0; i < nfds; i++){
            if(clientEvent[i].data.fd == serverSocket)
                setConnection(epollfd);
            else
                handleClientMessage(clientEvent[i].data.fd);
        }
    }
}

// Channel* Server::getChannelServ(const std::string& channelName) {
//     std::map<std::string, Channel*>::iterator it = channels.find(channelName);
//     if (it != channels.end()) {
//         return it->second;
//     }
//     return NULL;
// }

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
