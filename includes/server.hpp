/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/11 15:26:42 by ffilipe-         ###   ########.fr       */
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
#include "numeric_responses.hpp"

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
        std::vector<Channel* > serverChannels;   //Channels on sever

        void setEpoll();
        void createSocket();
        void bindSocket();
        void listenSocket();
        void setConnection(int epollfd);
        void handleClientMessage(int clientSocket);
        bool validateChannelModes(std::stringstream &iss, std::map<int, Client*>::iterator it, Channel *channel);
        Client *getClient(const std::string user);
        void notifyAllInChannel(Channel *channel, std::string message);
        void parseMessage(const std::string &message, std::map<int, Client*>::iterator it);
        void hNickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hUserCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hJoinCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hPartCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hPrivMsgCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hKickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hInviteCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hTopicCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hModeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hWhoCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void sendMsg(const std::string &msg, int clientSocket) const;
    public:
        Server();
        ~Server();
        
        void setupServer(char *port);
        
        // teste
        Channel* findOrCreateChannel(const std::string& channelName);
        void removeChannel(const std::string& channelName);
};

#endif