#include "client.hpp"
#include "server.hpp"

void Server::handleClientMessage(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0};
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        closeClientConnection(clientSocket);
    } else {
        std::string message(buffer);

        std::map<int, Client*>::iterator it = clients.find(clientSocket);
        if (it != clients.end()) {
            Client* client = it->second;  
            client->parseMessage(message);
        } else {
            std::cout << "Client not found for socket " << clientSocket << std::endl;
        }
    }
}

void Client::parseMessage(const std::string &message) {
    std::istringstream iss(message);
    std::string command;
    iss >> command;

    if (command == "NICK") {
        hNickCmd(iss);
    } else if (command == "USER") {
        hUserCmd(iss);
    } else if (command == "JOIN") {
        hJoinCmd(iss);
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

void Client::hNickCmd(std::istringstream &iss) {
    std::string nickname;
    iss >> nickname;
    setNick(nickname);
    std::cout << "Client " << _socket << " set nickname to " << nickname << "\n";
}


void Client::hUserCmd(std::istringstream &iss) {
    std::string username;
    iss >> username;
    setUser(username);
    std::cout << "Client " << _socket << " set username to " << username << "\n";
}

void Client::hJoinCmd(std::istringstream &iss) {
    std::string channel;
    iss >> channel;
    setChannel(channel);
    std::cout << "Client " << _socket << " joined channel " << channel << "\n";
}

void Client::hPrivMsgCmd(std::istringstream &iss) {
    std::string target, message;
    iss >> target;
    std::getline(iss, message);
    std::cout << "Private message from client " << _socket << " to " << target << ": " << message << "\n";

}

void Client::hKickCmd(std::istringstream &iss) {
    std::string target, reason;
    iss >> target >> reason;
    std::cout << "Client " << _socket << " kicked " << target << " for reason: " << reason << "\n";

}

void Client::hInviteCmd(std::istringstream &iss) {
    std::string target, channel;
    iss >> target >> channel;
    std::cout << "Client " << _socket << " invited " << target << " to channel " << channel << "\n";
}

void Client::hTopicCmd(std::istringstream &iss) {
    std::string topic;
    std::getline(iss, topic);
    std::cout << "Client " << _socket << " set channel topic to: " << topic << "\n";
}

void Client::hModeCmd(std::istringstream &iss) {
    std::string mode;
    iss >> mode;
    std::cout << "Client " << _socket << " changed mode to: " << mode << "\n";

}