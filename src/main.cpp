/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:01:10 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/15 18:28:39 by ffilipe-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

int main(int argc, char **argv)
{
    if(argc != 3) {
        std::cout << "Usage: ./ircserv [port] [password]\n";
        return 1;
    }
    else {
        try{
            Server server;
            server.setupServer(argv[1], argv[2]);
        }catch(std::exception &e){
            return EXIT_SUCCESS;
        }
    }
}