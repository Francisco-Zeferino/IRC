#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

void Server::setEpoll() {
    int epollfd = epoll_create1(0);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = serverSocket;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &event) == -1) {
        std::cerr << "Error adding server socket to epoll.\n";
        exit(1);
    }

    struct epoll_event clientEvent[1024];
    while (true) {
        int nfds = epoll_wait(epollfd, clientEvent, 1024, -1);
        if (nfds == -1) {
            std::cerr << "Error during epoll wait.\n";
            exit(1);
        }

        for (int i = 0; i < nfds; ++i) {
            if (clientEvent[i].data.fd == serverSocket) {
                setConnection(epollfd);
            } else {
                handleClientMessage(clientEvent[i].data.fd);
            }
        }
    }
}

Channel* Server::getChannelServ(const std::string& channelName) {
    std::map<std::string, Channel*>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        return it->second;
    }
    return NULL;
}

Channel* Server::findOrCreateChannel(const std::string& channelName) {
    std::map<std::string, Channel*>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        Channel* newChannel = new Channel(channelName);
        channels[channelName] = newChannel;
        std::cout << "Channel created: " << channelName << "\n";
        return newChannel;
    }
    return it->second;
}

void Server::removeChannel(const std::string& channelName) {
    std::map<std::string, Channel*>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        delete it->second;
        channels.erase(it);
        std::cout << "Channel " << channelName << " removed from server.\n";
    } else {
        std::cout << "Channel " << channelName << " not found.\n";
    }
}