/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:53:28 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/09/24 15:54:05 by ffilipe-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

Server::Server(){}

Server::~Server(){}

void Server::bindSocket(char *port){
    int opt = 1;
    std::cout << "Binding socket to port " << port << std::endl;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(port));
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        std::cout << "Error binding socket" << std::endl;
        exit(1);
    }
    if(listen(serverSocket, 1024) < 0){
        std::cout << "Error listening on socket" << std::endl;
        exit(1);
    }
    size_t serverAddrSize = sizeof(serverAddr);
    if((connection = accept(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t *)&serverAddrSize)) < 0){
        std::cout << "Error accepting connection" << std::endl;
        exit(1);
    }
    while(1){
        std::string msg;
        char buffer[1024] = {0};
        read(connection, buffer, 1024);
        std::cout << buffer << std::endl;
        std::getline(std::cin, msg);
        msg.append("\r\n");
        std::cout << msg << std::endl;
        write(connection, msg.c_str(), msg.length());
    }
}