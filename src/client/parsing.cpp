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
                it->second->parseMessage(commands[i], it);
            }
        else
            std::cout << "Client not found for socket " << clientSocket << std::endl;
    }
}

void Client::parseMessage(const std::string &message, std::map<int, Client*>::iterator it) {
    std::stringstream iss(message);
    std::string command;
    iss >> command;
    if (command == "NICK") {
        hNickCmd(iss);
    } else if (command == "USER") {
        hUserCmd(iss);
    } else if (command == "JOIN") {
        hJoinCmd(iss, it);
    } else if (command == "PART") {
        hPartCmd(iss, it);
    } else if (command == "PRIVMSG") {
        hPrivMsgCmd(iss);
    } else if (command == "KICK") {
        hKickCmd(iss);
    } else if (command == "INVITE") {
        hInviteCmd(iss);
    } else if (command == "TOPIC") {
        hTopicCmd(iss);
    } else if (command == "MODE") {
        hModeCmd(iss);
    } else {
        std::cout << "Unknown cmd: " << command << "\n";
    }
}

void Client::hNickCmd(std::stringstream &iss) {
    std::string nickname;
    iss >> nickname;
    setNick(nickname);
    std::cout << "Client " << _socket << " set nickname to " << nickname << "\n";
}

void Client::hUserCmd(std::stringstream &iss) {
    std::string username;
    iss >> username;
    setUser(username);
    std::cout << "Client " << _socket << " set username to " << username << "\n";
}


// Testar os modes
void Client::hJoinCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, pass;
    iss >> channelName >> pass;

    Channel* channel = server->findOrCreateChannel(channelName);  // Get or create the channel from the server
    // if (channel->hasMode('i')) {
    //     std::cout << "Channel " << channelName << " is invite-only.\n";
    //     return;
    // }

    // // +k
    // if (channel->hasMode('k') && !pass.empty() && channel->password != pass) {
    //     std::cout << "Incorrect password for channel " << channelName << "\n";
    //     return;
    // }

    // // +l
    // if (channel->hasMode('l') && channel->channelMember.size() >= channel->userslimit) {
    //     std::cout << "Channel " << channelName << " is full.\n";
    //     return;
    // }

    channel->addClient(this);
    std::cout << "Client " << _socket << " joined channel " << channelName << "\n";

    std::string message = ":" + getNick() + "!" + getUser() + "@localhost JOIN " + channelName + "\r\n";
    sendMsg(message, it->first);
}

void Client::hPartCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName;
    iss >> channelName;

    // Channel* channel = server->findOrCreateChannel(channelName);
    // if (!channel) {
    //     std::cout << "Channel not found: " << channelName << "\n";
    //     return;
    // }

    // Remove client from list
    // channel->removeClient(this);
    // std::cout << "Client " << _socket << " left channel " << channelName << "\n";

    // If the channel is now empty, remove it from the server's channels map
    // if (channel->channelMember.empty()) {
    //     server->removeChannel(channelName);  // Use the new removeChannel() method
    // }

    std::string message = ":" + getNick() + "!" + getUser() + "@localhost PART " + channelName + "\r\n";
    sendMsg(message, it->first);
}


void Client::hPrivMsgCmd(std::stringstream &iss) {
    std::string target, message;
    iss >> target;
    std::getline(iss, message);
    bool isChannel = (target[0] == '#'); 
    
    if (isChannel) {
        Channel* channel = server->findOrCreateChannel(target);

        if (!channel) {
            std::cout << "Channel not found: " << target << "\n";
            return;
        }

        for (std::map<Client*, bool>::iterator it = channel->channelMember.begin(); it != channel->channelMember.end(); ++it) {
            if (it->first != this) {
                it->first->sendMsg(":" + getNick() + "!" + getUser() + "@localhost PRIVMSG " + target + message + "\r\n", it->first->getSocket());
            }
        }
        std::cout << "Private message from client " << _socket << " to " << target << ": " << message << "\n";
    } else {
        std::cout << "Target is not a channel: " << target << "\n";
    }
}


void Client::hKickCmd(std::stringstream &iss) {
    std::string target, reason;
    iss >> target >> reason;
    std::cout << "Client " << _socket << " kicked " << target << " for reason: " << reason << "\n";

}

void Client::hInviteCmd(std::stringstream &iss) {
    std::string target, channel;
    iss >> target >> channel;
    std::cout << "Client " << _socket << " invited " << target << " to channel " << channel << "\n";
}

void Client::hTopicCmd(std::stringstream &iss) {
    std::string topic;
    std::getline(iss, topic);
    std::cout << "Client " << _socket << " set channel topic to: " << topic << "\n";
}

void Client::hModeCmd(std::stringstream &iss) {
    std::string channelName, mode;
    iss >> channelName >> mode;

    Channel* channel = server->getChannelServ(channelName);
    if (!channel) {
        std::cout << "Channel " << channelName << " not found.\n";
        return;
    }
    std::cout << "Channel " << channelName << " to seeeee.\n";

    // channel->applyMode(iss);
}