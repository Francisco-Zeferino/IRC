/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: struf <struf@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:07:21 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/09/24 18:17:20 by struf            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>

class Server
{
    private :
        int connection;
        int serverSocket;
        sockaddr_in serverAddr;
        
        int _port;
        std::string _password;

        
    public:
        Server();
        ~Server();
        void bindSocket(char *port);



        class IRCException : public std::exception
        {
        private:
            std::string _msg;
        public:
            IRCException(const std::string &message) : _msg(message) {}
        const char* what() const throw() { return _msg.c_str(); }
    };

    
        
};

#endif