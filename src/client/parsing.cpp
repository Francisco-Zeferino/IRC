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
        std::cout << "Unknown command: " << command << "\n";
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

void Client::hJoinCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channel;
    std::string message;
    iss >> channel;
    setChannel(channel);
    std::cout << "Client " << _socket << " joined channel " << channel << "\n";
    message = ":" + it->second->getNick() + "!" + it->second->getUser() + "@localhost JOIN " + it->second->getChannel() + " * :realname\r\n";
    channels.push_back(new Channel);
    channels.back()->setName(channel);
    std::cout << channels.back()->name << std::endl;
    sendMsg(message, it->first);
}

void Client::hPrivMsgCmd(std::stringstream &iss) {
    std::string target, message;
    iss >> target;
    std::getline(iss, message);
    std::cout << "Private message from client " << _socket << " to " << target << ": " << message << "\n";
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
    std::string mode;
    iss >> mode;
    std::cout << "Client " << _socket << " changed mode to: " << mode << "\n";

}