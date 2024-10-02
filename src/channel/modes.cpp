#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"


void Client::hModeCmd(std::stringstream &iss) {
    std::string channelName, mode;
    iss >> channelName >> mode;

    Channel* channel = NULL;
    std::map<std::string, Channel*>::iterator it = server->channels.find(channelName);
    if (it != server->channels.end()) {
        channel = it->second;
    } else {
        std::cout << "Channel not found: " << channelName << "\n";
        return;
    }

    if (!channel) {
        std::cout << "Channel not found: " << channelName << "\n";
        return;
    }

    if (channel->channelMember[this]) {
        channel->setMode(mode);

        channel->hModeCmd(mode, iss);

        std::cout << "Client " << _socket << " changed mode to: " << mode << " in channel " << channelName << "\n";
    } else {
        std::cout << "Client " << _socket << " is not an operator and cannot change modes.\n";
    }
}