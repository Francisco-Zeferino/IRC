/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:53:28 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/09/25 14:54:42 by ffilipe-         ###   ########.fr       */
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
    setConnection();
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

void Server::setConnection(){
    size_t serverAddrSize = sizeof(serverAddr);
    if((connection = accept(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t *)&serverAddrSize)) < 0){
        std::cout << "Error accepting connection" << std::endl;
        exit(1);
    }
    while(1){
        char msg[1024] = {0};
        read(connection, msg, 1024);
        std::cout << msg << std::endl;
    }
}