/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaptist <mbaptist@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/14 15:31:48 by mbaptist         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <poll.h>
#include <fcntl.h> 
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <map>
#include <vector>
#include <sys/epoll.h>
#include "numeric_responses.hpp"

class Client;
class Channel;

class Server
{
    private :
        int connection;
        int serverSocket;
        char *_port;
        
        char *password;
        sockaddr_in serverAddr;
        std::map<int, Client* > clients;            //Clients connected to server
        std::vector<Channel* > serverChannels;      //Channels on sever

        void setEpoll();
        void createSocket();
        void bindSocket();
        void listenSocket();
        void setConnection(int epollfd);
        void handleClientMessage(int clientSocket);
        bool validateChannelModes(std::stringstream &iss, std::map<int, Client*>::iterator it, Channel *channel);
        Client *getClient(const std::string user);

        // Command handling
        void parseMessage(const std::string &message, std::map<int, Client*>::iterator it);
        void hNickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hUserCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hJoinCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hPartCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hPrivMsgCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hKickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hInviteCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hModeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hWhoCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hPassCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hTopicCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        
    public:
        Server();
        ~Server();
        
        void setupServer(char *port, char *password);
        
        // teste
        Channel* findOrCreateChannel(const std::string& channelName);
        void removeChannel(const std::string& channelName);
};

#endif