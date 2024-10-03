/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/03 18:12:07 by ffilipe-         ###   ########.fr       */
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
#include <vector>
#include <sys/epoll.h>

class Client;
class Channel;

class Server
{
    private :
        int connection;
        int serverSocket;
        char *_port;
        sockaddr_in serverAddr;
        
        std::map<int, Client* > clients;            //Clients connected to server
        std::vector<Channel *> channels;   //Channels on sever

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
        
        // teste
        Channel* findOrCreateChannel(const std::string& channelName);
        Channel* getChannelServ(const std::string& channelName);
        void removeChannel(const std::string& channelName);
};

#endif