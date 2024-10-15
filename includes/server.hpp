/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/15 18:24:35 by ffilipe-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <cstring>
#include <csignal>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <map>
#include <vector>
#include <sys/epoll.h>
#include <errno.h>
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
        int epollfd;
        int nfds;
        epoll_event event;
        std::map<int, Client* > clients;            //Clients connected to server
        std::vector<Channel* > serverChannels;      //Channels on sever

        //Socket Management
        void setEpoll();
        void createSocket();
        void bindSocket();
        void listenSocket();
        void setConnection(int epollfd);
        static void handleSignal(int signal);
        
        //Epoll state management
        void epollState(int epollfd, int socket, uint32_t newEvent);

        //Client management
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
        void hTopicCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hModeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hWhoCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hPassCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        
        
    public:
        Server();
        ~Server();
        
        void setupServer(char *port, char *password);
        
        // teste
        Channel* findOrCreateChannel(const std::string& channelName);
        void removeChannel(const std::string& channelName);
};

#endif