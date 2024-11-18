#include "client.hpp"
#include "server.hpp"
#include "channel.hpp"

void Server::hNickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string nickname;
    iss >> nickname;

    if (nickname.empty() || nickname.length() > 9 || !std::isalnum(nickname[0])) {
        sendMsgServ(ERR_ERRONEUSNICKNAME(it->second->getNick(), nickname), it->first);
        return;
    }

    for (std::map<int, Client*>::iterator clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
        if (clientIt->second->getNick() == nickname) {
            sendMsgServ(ERR_NICKNAMEINUSE(it->second->getNick(), nickname), it->first);
            return;
        }
    }

    std::string oldNick = it->second->getNick();
    it->second->setNick(nickname);

    sendMsgServ(":localhost NICK " + nickname + "\r\n", it->first);
    for (std::vector<Channel*>::iterator chIt = it->second->clientChannels.begin(); chIt != it->second->clientChannels.end(); ++chIt) {
        std::string message = ":" + oldNick + "!" + it->second->getUser() + "@localhost NICK " + nickname + "\r\n";
        (*chIt)->notifyAllInChannel(*chIt, message);
    }
}

void Server::hUserCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string username;
    iss >> username;

    it->second->setUser(username);
    std::string welcomeMsg = RPL_WELCOME(user_info(it->second->getNick(), it->second->getUser()), it->second->getNick());
    sendMsgServ(welcomeMsg, it->first);


    std::string nickChangeMsg = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost NICK " + it->second->getNick() + "\r\n";
    for (std::vector<Channel*>::iterator channelIt = it->second->clientChannels.begin();
         channelIt != it->second->clientChannels.end(); ++channelIt) {
        
        Channel* channel = *channelIt;
        channel->notifyAllInChannel(channel, nickChangeMsg);
    }
}

std::string getRole(std::map<Client *, bool>::iterator it){
    if(it->second == true)
        return " :@";
    else
        return " :";
}

void Server::hWhoCmd(std::stringstream &iss, std::map<int, Client*>::iterator it){
   std::string channelName;
   iss >> channelName;
   std::map<Client *, bool>::iterator clientIt;
   std::vector<Bot >::iterator botIt;
   Channel *channel= findChannel(channelName);
   if (!channel) {
        std::cout << "Channel not found: " << channelName << "\n";
        return;
    }
    for(clientIt = channel->admins.begin(); clientIt != channel->admins.end(); clientIt++){
        if(it->second->getNick() != clientIt->first->getNick())
            channel->sendMsg(":localhost 353 " + it->second->getNick() + " @ " + channelName + getRole(clientIt) + clientIt->first->getNick() + "\r\n", it->first);
        else
            channel->sendMsg(":localhost 353 " + it->second->getNick() + " @ " + channelName + getRole(clientIt) + it->second->getNick() + "\r\n", it->first);
    }
    for(botIt = channel->bots.begin(); botIt != channel->bots.end(); botIt++){
        channel->sendMsg(":localhost 353 " + it->second->getNick() + " @ " + channelName + " :+" + (botIt)->getNick() + "\r\n", it->first);
    }
    channel->sendMsg(":localhost 366 " + it->second->getNick() + " " + channelName + " :End of /NAMES list.\r\n", it->first);
}

void Server::hPartCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, reason;
    iss >> channelName;

    Channel* channel = findChannel(channelName);
    if (!channel) {
        channel->sendMsg(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    if (channel->admins.find(it->second) == channel->admins.end()) {
        channel->sendMsg(ERR_NOTONCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    std::getline(iss, reason);
    if (!reason.empty() && reason[0] == ' ') {
        reason = reason.substr(1);
    }

    std::string message = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost PART " + channelName;
    if (!reason.empty()) {
        message += " :" + reason;
    }
    message += "\r\n";
    channel->notifyAllInChannel(channel, message);

    channel->removeClient(it->second);
}

void Server::hPrivMsgCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string target, message;
    iss >> target;
    std::getline(iss, message);
    bool isChannel = (target[0] == '#'); 
    
    if (isChannel) {
        Channel* channel = findChannel(target);
        if (!channel) {
            std::cout << "Channel not found: " << target << "\n";
            return;
        }
        for (std::map<Client*, bool>::iterator channelMap = channel->admins.begin(); channelMap != channel->admins.end(); channelMap++) {
            if (it->second != channelMap->first) {
                channel->sendMsg(RPL_PRIVMSG(user_info(it->second->getNick(), it->second->getUser()), target, message), channelMap->first->getSocket());
            }
        }
        std::cout << "Private message from client " << it->second->getSocket() << " to " << target << ": " << message << "\n";
    } else {
        std::map<int, Client*>::iterator clientIt = clients.begin();
        while(clientIt != clients.end()){
            if(target == clientIt->second->getNick())
                sendMsgServ(RPL_PRIVMSG(user_info(clientIt->second->getNick(), clientIt->second->getUser()), target, message), clientIt->first);
            clientIt++;
        }
    }
}

void Server::hNoticeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string target, message;
    iss >> target;
    std::getline(iss, message);
    bool isChannel = (target[0] == '#'); 
    
    if (isChannel) {
        Channel* channel = findChannel(target);
        if (!channel) {
            std::cout << "Channel not found: " << target << "\n";
            return;
        }
        for (std::map<Client*, bool>::iterator channelMap = channel->admins.begin(); channelMap != channel->admins.end(); channelMap++) {
            if (it->second != channelMap->first) {
                channel->sendMsg(RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), target, message), channelMap->first->getSocket());
            }
        }
    } else {
        std::map<int, Client*>::iterator clientIt = clients.begin();
        while(clientIt != clients.end()){
            if(target == clientIt->second->getNick())
                sendMsgServ(RPL_NOTICE(user_info(clientIt->second->getNick(), clientIt->second->getUser()), target, message), clientIt->first);
            clientIt++;
        }
    }

}

void Server::hKickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, targetNick, reason;
    iss >> channelName >> targetNick;

    Channel* channel = findChannel(channelName);
    if (!channel) {
        channel->sendMsg(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    if (!channel->isAdmin(it->second)) {
        channel->sendMsg(ERR_CHANOPRIVSNEEDED(it->second->getNick(), channelName), it->first);
        return;
    }

    Client* targetClient = NULL;
    for (std::map<Client*, bool>::iterator clientIt = channel->admins.begin(); clientIt != channel->admins.end(); ++clientIt) {
        if (clientIt->first->getNick() == targetNick) {
            targetClient = clientIt->first;
            break;
        }
    }
    if (!targetClient) {
        channel->sendMsg(ERR_USERNOTINCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    if (iss.rdbuf()->in_avail() > 0) {
        std::getline(iss, reason);
        if (!reason.empty() && reason[0] == ' ') {
            reason = reason.substr(1);
        }
    }

    std::string kickMessage = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    channel->notifyAllInChannel(channel, kickMessage);
    channel->removeClient(targetClient);
}

void Server::hInviteCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string targetNick, targetChannel;
    iss >> targetNick >> targetChannel;

    Channel* channel = findChannel(targetChannel); //alt

    if (!channel->isAdmin(it->second)) {
        channel->sendMsg(ERR_CHANOPRIVSNEEDED(it->second->getNick(), targetChannel), it->first);
        return;
    }

    if (!channel->hasMode('i')) {
        channel->sendMsg(":localhost 341 " + it->second->getNick() + " " + targetChannel + " :Channel is not invite-only.\r\n", it->first);
        return;
    }

    Client* targetClient = getClient(targetNick);
    if (!targetClient) {
        channel->sendMsg(ERR_NOSUCHNICK(it->second->getNick(), targetNick), it->first);  // No such user
        return;
    }

    if (std::find(channel->invUsers.begin(), channel->invUsers.end(), targetNick) != channel->invUsers.end()) {
        channel->sendMsg(":localhost 443 " + it->second->getNick() + " " + targetNick + " :User is already in channel.\r\n", it->first);
        return;
    }
    channel->invUsers.push_back(targetNick);

    std::cout << "Client " << it->second->getSocket() << " invited " << targetNick << " to channel " << targetChannel << "\n";

    std::string inviting = RPL_INVITING(user_info(it->second->getNick(), it->second->getUser()), targetNick, targetChannel);
    std::string inviteMsg = RPL_INVITE(user_info(it->second->getNick(), it->second->getUser()), targetNick, targetChannel);

    sendMsgServ(inviteMsg, targetClient->getSocket());
    channel->notifyAllInChannel(channel, inviting);
}

void Server::hTopicCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, newTopic;
    iss >> channelName;

    Channel* channel = findChannel(channelName);
    if (!channel) {
        channel->sendMsg(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    if (iss >> std::ws && std::getline(iss, newTopic)) {
        if (!newTopic.empty() && newTopic[0] == ' ') {
            newTopic = newTopic.substr(1);
        }
        if (!newTopic.empty() && newTopic[0] == ':') {
            newTopic = newTopic.substr(1);
        }

        channel->setTopic(newTopic, it->second);
    } else {
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty()) {
            channel->sendMsg(RPL_NOTOPIC(it->second->getNick(), channelName), it->first);
        } else {
            channel->sendMsg(RPL_TOPIC(it->second->getNick(), channelName, currentTopic), it->first);
        }
    }
}

void Server::hQuitCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string quitMessage;
    std::getline(iss, quitMessage);
    std::cout << "QUIT 1\n";
    if (quitMessage.empty()) {
        quitMessage = "Client Quit";
    }

    Client* client = it->second;
    for (std::vector<Channel*>::iterator ch = client->clientChannels.begin(); ch != client->clientChannels.end(); ch++) {
        std::string message = RPL_PART(user_info(client->getNick(), client->getUser()), (*ch)->name);
        (*ch)->notifyAllInChannel(*ch, message);
    }
    for (std::vector<Channel*>::iterator ch = client->clientChannels.begin(); ch != client->clientChannels.end(); ++ch) {
        (*ch)->removeClient(client);
    }

    sendMsgServ("ERROR :Closing Link: " + client->getNick() + " (" + quitMessage + ")\r\n", it->first);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, it->first, NULL);
    close(it->first);
    delete it->second;
    clients.erase(it);
}

