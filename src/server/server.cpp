/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:53:28 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/09/26 16:27:01 by ffilipe-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/server.hpp"

Server::Server(){}

Server::~Server(){}

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
    if(listen(serverSocket, 1024) < 0){
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
            if(clientEvent[i].data.fd == serverSocket){
                setConnection(epollfd);
            }
            else{
                handleClients(clientEvent[i].data.fd);
            }
        }
    }
}

void Server::setConnection(int epollfd){
    size_t serverAddrSize = sizeof(serverAddr);
    if((connection = accept(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t *)&serverAddrSize)) < 0){
        std::cout << "Error accepting connection" << std::endl;
        exit(1);
    }
    struct epoll_event connectionEvent;
    connectionEvent.events = EPOLLIN;
    connectionEvent.data.fd = connection;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, connection, &connectionEvent);
}

void Server::handleClients(int clientConnection){;
    char buffer[1024] = {0};
    int readBytes = read(clientConnection, buffer, 1024);
    if(readBytes == 0){
        close(clientConnection);
    }
    else if(readBytes < 0){
        std::cout << "Error reading from client" << std::endl;
        exit(1);
    }
    else{
        std::cout << "Client says: " << buffer << std::endl;
        //send(clientConnection, buffer, strlen(buffer), 0);
    }
}