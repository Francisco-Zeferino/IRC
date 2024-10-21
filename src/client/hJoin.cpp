#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

void Server::hJoinCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName;
    iss >> channelName;

    bool isChannel = (channelName[0] == '#');
    if (isChannel) {
        Channel* channel = findOrCreateChannel(channelName);
        
        if (validateChannelModes(iss, it, channel)) {
            return;
        }

        std::string message = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost JOIN " + channelName + "\r\n";
        std::cout << "Client " << it->second->getSocket() << " joined channel " << channelName << "\n";
        
        if (channel->admins.empty()) {
            channel->addClient(it->second, true);
        } else {
            channel->addClient(it->second);
        }

        channel->sendMsg(message, it->first);
        channel->notifyAllInChannel(channel, message);
    } else {
        std::cout << channelName << " is not a valid channel name\n";
    }
}



bool Server::validateChannelModes(std::stringstream &iss, std::map<int, Client*>::iterator it, Channel* channel) {
    if (channel->hasMode('l') && channel->admins.size() >= channel->userslimit) {
        channel->sendMsg(ERR_CHANNELISFULL(it->second->getNick(), channel->name), it->first);
        return true;
    }

    if (channel->hasMode('i')) {
        if (!channel->validateUserJoin(it->second->getNick())) {
            channel->sendMsg(ERR_INVITEONLYCHAN(it->second->getNick(), channel->name), it->first);
            return true;
        }
    }

    if (channel->hasMode('k')) {
        std::cout << "Here" << std::endl;
        std::string providedPassword;
        iss >> providedPassword;
        if (channel->password != providedPassword) {
            channel->sendMsg(ERR_BADCHANNELKEY(it->second->getNick(), channel->name), it->first);
            return true;
        }
    }
    return false;
}
