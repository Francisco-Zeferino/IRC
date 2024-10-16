#include "client.hpp"
#include "server.hpp"
#include "channel.hpp"

std::vector<std::string> splitCommands(const std::string &message) {
    std::vector<std::string> commands;
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    while ((end = message.find("\n", start)) != std::string::npos) {
        std::string command = message.substr(start, end - start);
        if (!command.empty()) {
            commands.push_back(command);
        }
        start = end + 1;
    }
    return commands;
}

std::string getClientMessage(int clientSocket, int &bytesRead) {
    char buffer[BUFFER_SIZE] = {0};
    bool stopLoop = false;
    std::string tmp;
    while(!stopLoop){
        memset(buffer, 0, sizeof(buffer));
        bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        tmp.append(std::string(buffer));
        if(tmp.find('\n') != std::string::npos)
            stopLoop = true;
    }
    return tmp;
}

void Server::handleClientMessage(int clientSocket) {
    std::string clientMessage;
    int bytesRead;
    clientMessage = getClientMessage(clientSocket, bytesRead);
    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    } else {
        std::vector<std::string> commands;
        std::string message(clientMessage);
        commands = splitCommands(message);
        std::map<int, Client*>::iterator it = clients.find(clientSocket);
        if (it != clients.end())
            for (size_t i = 0; i < commands.size(); ++i) {
                parseMessage(commands[i], it);
            }
        else
            std::cout << "Client not found for socket " << clientSocket << std::endl;
    }
}

void Server::parseMessage(const std::string &message, std::map<int, Client*>::iterator it) {
    std::stringstream iss(message);
    std::string command;
    iss >> command;
    if (command == "NICK") {
        hNickCmd(iss, it);
    } else if (command == "USER") {
        hUserCmd(iss, it);
    } else if (command == "JOIN") {
        hJoinCmd(iss, it);
    } else if (command == "PART") {
        hPartCmd(iss, it);
    } else if (command == "PRIVMSG") {
        hPrivMsgCmd(iss, it);
    } else if (command == "KICK") {
        hKickCmd(iss, it);
    } else if (command == "INVITE") {
        hInviteCmd(iss, it);
    } else if (command == "TOPIC") {
        hTopicCmd(iss, it);
    } else if (command == "MODE") {
        hModeCmd(iss, it);
    } else if (command == "WHO") {
        hWhoCmd(iss, it);
    } else if (command == "PASS") {
        hPassCmd(iss, it);
    } else if (command == "QUIT") {
        // hQuitCmd(iss, it);
    }else
        return;
}

void Server::hNickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string nickname;
    iss >> nickname;
    it->second->setNick(nickname);
}

void Server::hUserCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string username;
    iss >> username;
    it->second->setUser(username);
    std::string msg = RPL_WELCOME(user_info(it->second->getNick(), it->second->getUser()), it->second->getNick());
    send(it->first, msg.c_str(), msg.length(), 0);
    //channel->sendMsg(RPL_WELCOME(user_info(it->second->getNick(), it->second->getUser()), it->second->getNick()), it->first);
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
   Channel *channel= findOrCreateChannel(channelName);
    for(clientIt = channel->admins.begin(); clientIt != channel->admins.end(); clientIt++){
        if(it->second->getNick() != clientIt->first->getNick())
            channel->sendMsg(":localhost 353 " + it->second->getNick() + " @ " + channelName + getRole(clientIt) + clientIt->first->getNick() + "\r\n", it->first);
        else
            channel->sendMsg(":localhost 353 " + it->second->getNick() + " @ " + channelName + getRole(clientIt) + it->second->getNick() + "\r\n", it->first);
    }
   channel->sendMsg(":localhost 366 " + it->second->getNick() + " " + channelName + " :End of /NAMES list.\r\n", it->first);
}

void Server::hPartCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName;
    std::map<Client *, bool>::iterator mapIterator;
    iss >> channelName;

    Channel* channel = findOrCreateChannel(channelName);

    std::cout << "Client " << it->second->getSocket() << " left channel " << channelName << "\n";

    // If the channel is now empty, remove it from the server's channels map
    if (channel->admins.empty()) {
          // Use the new removeChannel() method
    }
    std::string message = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost PART " + channelName + "\r\n";
    channel->notifyAllInChannel(channel, message);
    // mapIterator = channel->admins.find(it->second);
    // (*mapIterator).second = false;
    // mapIterator++;
    // (*mapIterator).second = true;
}

void Server::hPrivMsgCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string target, message;
    iss >> target;
    std::getline(iss, message);
    bool isChannel = (target[0] == '#'); 
    
    if (isChannel) {
        Channel* channel = findOrCreateChannel(target);
        if (!channel) {
            std::cout << "Channel not found: " << target << "\n";
            return;
        }
        for (std::map<Client*, bool>::iterator channelMap = channel->admins.begin(); channelMap != channel->admins.end(); channelMap++) {
            if (it->second != channelMap->first) {
                channel->sendMsg(":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost PRIVMSG " + target + message + "\r\n", channelMap->first->getSocket());
            }
        }
        std::cout << "Private message from client " << it->second->getSocket() << " to " << target << ": " << message << "\n";
    } else {
        std::map<int, Client*>::iterator clientIt = clients.begin();
        while(clientIt != clients.end()){
            if(target == clientIt->second->getNick())
                // channel->sendMsg(":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost PRIVMSG " + target + message + "\r\n", clientIt->first);
            clientIt++;
        }
    }
}

void Server::hKickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string target, reason;
    iss >> target >> reason;
    std::cout << "Client " << it->second->getSocket() << " kicked " << target << " for reason: " << reason << "\n";

}

void Server::hInviteCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string target, targetChannel;
    iss >> target >> targetChannel;
    Channel *channel = findOrCreateChannel(targetChannel);
    Client *client = getClient(target);
    channel->invUsers.push_back(target);
    std::cout << "Client " << it->second->getSocket() << " invited " << target << " to channel " << targetChannel << "\n";
    std::string inviting = RPL_INVITING(user_info(it->second->getNick(), it->second->getUser()), target, targetChannel);
    std::string msg = RPL_INVITE(user_info(it->second->getNick(), it->second->getUser()), target, targetChannel);
    channel->notifyAllInChannel(channel, inviting);
    channel->sendMsg(msg, client->getSocket());
}


void Server::hTopicCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, newTopic;
    iss >> channelName;

    Channel* channel = findOrCreateChannel(channelName);
    if (!channel) {
        channel->sendMsg(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    // If no new topic is provided, return the current topic
    if (iss.rdbuf()->in_avail() == 0) {
        std::string currentTopic = channel->getTopic();
        if (currentTopic == "No topic is set") {
            channel->sendMsg(RPL_NOTOPIC(it->second->getNick(), channelName), it->first);
        } else {
            channel->sendMsg(RPL_TOPIC(it->second->getNick(), channelName, currentTopic), it->first);
        }
        return;
    }

    std::getline(iss, newTopic);
    newTopic = newTopic.substr(1); 
    channel->setTopic(newTopic, it->second);
    //notify
}
