/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/09/26 16:23:28 by ffilipe-         ###   ########.fr       */
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
#include <sys/epoll.h>

class Server
{
    private :
        int connection;
        int serverSocket;
        char *_port;
        sockaddr_in serverAddr;
        void setEpoll();
        void createSocket();
        void bindSocket();
        void listenSocket();
        void setConnection(int epollfd);
        void handleClients(int clientConnection);
    public:
        Server();
        ~Server();
        void setupServer(char *port);
};

#endif