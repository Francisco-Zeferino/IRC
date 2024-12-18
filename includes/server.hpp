/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaptist <mbaptist@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/11/20 14:49:21 by mbaptist         ###   ########.fr       */
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
#include <csignal>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <map>
#include <vector>
#include <sys/epoll.h>
#include <errno.h>
#include <ctime>
#include <iomanip>

#include "numeric_responses.hpp"

class Client;
class Channel;
class Bot;

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
        std::stringstream raw;

        //Socket Management
        void setEpoll();
        void createSocket();
        void bindSocket();
        void listenSocket();
        void setConnection(int epollfd);
        static void handleSignal(int signal);
        void closeConnections();
        void handleQuitOnSignal();
        
        //Bot
        int setBot();
        Bot createBot();
        void aBotJoin(std::map<int, Client*>::iterator it, const std::string &channelName);
        void aBotHelp(std::map<int, Client*>::iterator it, const std::string &channelName);
        void aBotLeave(std::map<int, Client*>::iterator it, const std::string &channelName);
        void aBotHello(std::map<int, Client*>::iterator it, const std::string &channelName);
        void aBotTime(std::map<int, Client*>::iterator it, const std::string &channelName);
        void aBotHangman(std::map<int, Client*>::iterator it, const std::string &channelName, std::stringstream &iss);
        
        //Epoll state management
        void epollState(int epollfd, int socket, uint32_t newEvent);

        //Client management
        void handleClientMessage(int clientSocket);
        bool validateChannelModes(std::stringstream &iss, std::map<int, Client*>::iterator it, Channel *channel);
        Client *getClient(const std::string user);
        Client *getClientByFd(int socketFd);
        bool getClientMessage(int clientSocket, int &bytesRead);

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
        void hNoticeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hSFCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hSFACmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hQuitCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void hBotCmd(std::stringstream &iss, std::map<int, Client*>::iterator it);
        void sendMsgServ(const std::string &msg, int clientSocket) const;
        void startDcc(const std::string fileName, const std::string savedFileName);
        void setReceiver(size_t fileSize, std::string fileName);
        bool isClientAuthenticated(Client *client);
        
    public:
        Server();
        ~Server();
        
        void setupServer(char *port, char *password);
        
        Channel* findChannel(const std::string& channelName);
        Channel* createChannel(const std::string& channelName);
        void removeChannel(const std::string& channelName);

};  

#endif