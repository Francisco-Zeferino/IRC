#include "client.hpp"
#include "server.hpp"
#include "channel.hpp"

std::vector<std::string> splitCommands(const std::string &message) {
    std::vector<std::string> commands;
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    while ((end = message.find("\r\n", start)) != std::string::npos) {
        std::string command = message.substr(start, end - start);
        if (!command.empty()) {
            commands.push_back(command);
        }
        start = end + 2;
    }
    return commands;
}

void Server::handleClientMessage(int clientSocket) {
    char buffer[BUFFER_SIZE * BUFFER_SIZE] = {0};
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        close(clientSocket);
    } else {
        std::vector<std::string> commands;
        std::string message(buffer);
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
    }else{
        std::cout << "Unknown cmd: " << command << "\n";
    }
}

void Server::hNickCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string nickname;
    iss >> nickname;
    it->second->setNick(nickname);
    std::cout << "Client " << it->second->getSocket() << " set nickname to " << nickname << "\n";
}

void Server::hUserCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string username;
    iss >> username;
    it->second->setUser(username);
    std::cout << "Client " << it->second->getSocket() << " set username to " << username << "\n";
}

void Server::hWhoCmd(std::stringstream &iss, std::map<int, Client*>::iterator it){
   std::string channelName;
   iss >> channelName;
   std::vector<Client *>::iterator clientIt;
   Channel *channel= findOrCreateChannel(channelName);
    for(clientIt = channel->clientsInChannel.begin(); clientIt != channel->clientsInChannel.end(); clientIt++){
        if(it->second->getNick() != (*clientIt)->getNick())
            sendMsg(":localhost 353 " + it->second->getNick() + " @ " + channelName + " :+" + it->second->getNick() + " " + (*clientIt)->getNick() + "\r\n", it->first);
        else
            sendMsg(":localhost 353 " + it->second->getNick() + " @ " + channelName + " :+" + it->second->getNick() + "\r\n", it->first);
    }
   sendMsg(":localhost 366 " + it->second->getNick() + " " + channelName + " :End of /NAMES list.\r\n", it->first);
}

// Testar os modes
void Server::hJoinCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, pass;
    iss >> channelName;
    bool isChannel = (channelName[0] == '#');
    std::vector<Client *>::iterator clientIt;
    if(isChannel){
        Channel *channel = findOrCreateChannel(channelName);  // Get or create the channel from the server
        if(validateChannelModes(iss, it, channel) == false)
        {
            std::string message = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost JOIN " + channelName + "\r\n";
            std::cout << "Client " << it->second->getSocket() << " joined channel " << channelName << "\n";
            channel->addClient(it->second);
            sendMsg(message, it->first);
            for(clientIt = channel->clientsInChannel.begin(); clientIt != channel->clientsInChannel.end(); clientIt++){
                if(it->second->getNick() != (*clientIt)->getNick())
                    sendMsg(message, (*clientIt)->getSocket());
                }
        }
    }
    else
        std::cout << channelName << " is not channel" << std::endl;
}

void Server::hPartCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName;
    iss >> channelName;

    // Channel* channel = server->findOrCreateChannel(channelName);
    // if (!channel) {
    //     std::cout << "Channel not found: " << channelName << "\n";
    //     return;
    // }

    // Remove client from list
    // channel->removeClient(this);
    // std::cout << "Client " << it->second->getSocket() << " left channel " << channelName << "\n";

    // If the channel is now empty, remove it from the server's channels map
    // if (channel->channelMember.empty()) {
    //     server->removeChannel(channelName);  // Use the new removeChannel() method
    // }

    std::string message = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost PART " + channelName + "\r\n";
    sendMsg(message, it->first);
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

        for (std::vector<Client*>::iterator vectorChannel = channel->clientsInChannel.begin(); vectorChannel != channel->clientsInChannel.end(); vectorChannel++) {
            if (it->second != *vectorChannel) {
                sendMsg(":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost PRIVMSG " + target + message + "\r\n", (*vectorChannel)->getSocket());
            }
        }
        std::cout << "Private message from client " << it->second->getSocket() << " to " << target << ": " << message << "\n";
    } else {
        std::map<int, Client*>::iterator clientIt = clients.begin();
        while(clientIt != clients.end()){
            if(target == clientIt->second->getNick())
                sendMsg(":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost PRIVMSG " + target + message + "\r\n", clientIt->first);
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
    std::string msg = ":localhost " + it->second->getNick() + " " + targetChannel + ":invited to channel (+i)" + "\r\n";
    sendMsg(msg, client->getSocket());
}

void Server::hTopicCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string topic;
    std::getline(iss, topic);
    std::cout << "Client " << it->second->getSocket() << " set channel topic to: " << topic << "\n";
}

void Server::hModeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, mode, password;
    iss >> channelName >> mode;
    (void)it;
    Channel* channel = findOrCreateChannel(channelName);
    if (!channel) {
        std::cout << "Channel " << channelName << " not found.\n";
        return;
    }
    channel->applyMode(iss, mode);
}