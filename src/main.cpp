/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffilipe- <ffilipe-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 15:01:10 by ffilipe-          #+#    #+#             */
/*   Updated: 2024/09/23 16:35:47 by ffilipe-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

int main(int argc, char **argv)
{
    if(argc != 3){
        std::cout << "Usage: ./ircserv [port] [password]" << std::endl;
        return 1;
    }else{
        Server server;
        server.bindSocket(argv[1]);
        return 0;
    }
}