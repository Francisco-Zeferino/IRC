/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:01:10 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/10/11 18:41:00 by ffilipe-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

int main(int argc, char **argv)
{
    if(argc != 3) {
        std::cout << "Usage: ./ircserv [port] [password]" << std::endl;
        return 1;
    }
    else {
        Server server;
        server.setupServer(argv[1], argv[2]); //add pass AV2
        return 0;
    }
}