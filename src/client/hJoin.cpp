#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

void Server::hJoinCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName;
    iss >> channelName;

    bool isChannel = (channelName[0] == '#');
    if (!isChannel) {
        std::cout << channelName << " is not a valid channel name\n";
        return;
    }

    Channel* channel = findChannel(channelName);
    if (channel == NULL) {
        channel = createChannel(channelName);
        if (channel == NULL) {
            std::cerr << "Failed to create channel: " << channelName << "\n";
            return;  
        }
    }
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
    it->second->clientChannels.push_back(channel);

    //BOT
    if (channel->bot == NULL) {
        std::cout << "BoT START\n"; 
        channel->bot = startBot(channel);
        
        if (channel->bot) {
            channel->addClient(channel->bot);
            std::string botJoinMessage = ":" + channel->bot->getNick() + "!" + channel->bot->getUser() + "@localhost JOIN " + channelName + "\r\n";
            channel->notifyAllInChannel(channel, botJoinMessage);
        } else {
            std::cerr << "Failed to start bot for channel: " << channelName << "\n";
        }
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

void Server::hPassCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string receivedPassword;
    iss >> receivedPassword;

    if (this->password != receivedPassword) {
        sendMsgServ(ERR_PASSWDMISMATCH(), it->first);
        close(it->first);
        std::cout << "Client disconnected due to incorrect password\n";
        
    } else {
        std::cout << "Client " << it->second->getNick() << " authenticated with correct server password\n";
    }
    
}