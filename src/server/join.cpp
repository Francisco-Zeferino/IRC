#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

// Testar os modes
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
        
        if (channel->admins.size() == 0) {
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


/*// troquei os elses para ifs
bool Server::validateChannelModes(std::stringstream &iss, std::map<int, Client*>::iterator it, Channel *channel){
    std::string message = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost JOIN " + channel->name + "\r\n";
    
    // +i
    if(channel->hasMode('i')){
        if(channel->validateUserJoin(it->second->getNick())){
            std::cout << "Client " << it->second->getSocket() << " joined channel " << channel->name << "\n";
            channel->addClient(it->second);
            notifyAllInChannel(channel, message);
            return true;
        }
        else {
            std::cout << "Can't join Channel, not invited!" << std::endl;
            sendMsg(ERR_INVITEONLYCHAN(it->second->getNick(), channel->name), it->first);
            return true;
        }
    }
    
    // +k
    else if(channel->hasMode('k')) {
        std::string providedPassword;
        iss >> providedPassword;

        if (channel->password == providedPassword) {
            std::cout << "Client " << it->second->getSocket() << " joined channel " << channel->name << " with correct password\n";
            channel->addClient(it->second);
            sendMsg(message, it->first);
            return true;
        } else {
            std::cout << "Wrong password for channel " << channel->name << "\n";
            sendMsg(ERR_PASSWDMISMATCH(), it->first);
            return true;
    }
}
    
    //+l
    else if(channel->hasMode('l')){
        if(channel->admins.size() >= channel->userslimit){
            std::string message = ERR_CHANNELISFULL(it->second->getNick(), channel->name);
            sendMsg(message,it->first);
            return true;
        }
        else{
            std::cout << "Client " << it->second->getSocket() << " joined channel " << channel->name << "\n";
            channel->addClient(it->second);
            sendMsg(message, it->first);
            return true;
        }
    }
    return false;
}*/