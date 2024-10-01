/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/01 16:16:07 by ffilipe-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <map>
#include <sys/epoll.h>

class Client;

class Server
{
    private :
        int connection;
        int serverSocket;
        char *_port;
        sockaddr_in serverAddr;
        std::map<int, Client* > clients;
        void setEpoll();
        void createSocket();
        void bindSocket();
        void listenSocket();
        void setConnection(int epollfd);
        void handleClientMessage(int clientSocket);
    public:
        Server();
        ~Server();
        void setupServer(char *port);
};

#endif