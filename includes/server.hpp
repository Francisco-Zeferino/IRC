/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: struf <struf@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/09/29 00:09:02 by struf            ###   ########.fr       */
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
#include "client.hpp"

class Client;

class Server
{
    private :
        int connection;
        int serverSocket;
        char *_port;
        sockaddr_in serverAddr;
        
        std::string _password;
        std::vector<pollfd> pollfds;
        std::map<int, Client*> clients;

        
    public:
        Server();
        ~Server();
        void setupServer(char *port);
        void createSocket();
        void bindSocket();
        void listenSocket();
        // void setConnection();

        // MC
        void run();
        void handleNewConnection();
        void handleClientMessage(int clientSocket);
        void closeClientConnection(int clientSocket);
   


        
};

#endif