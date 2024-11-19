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

Client *Server::getClientByFd(int socketFd){
    return clients.find(socketFd)->second;
}

bool Server::getClientMessage(int clientSocket, int &bytesRead) {
    Client *client = getClientByFd(clientSocket);
    char buffer[BUFFER_SIZE] = {0};
    memset(buffer, 0, sizeof(buffer));
    bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if(bytesRead <= 0){
        std::stringstream s;
        hQuitCmd(s, clients.find(clientSocket));
        return false; 
    }
    client->messageBuffer.append(buffer);
    if(client->messageBuffer.find("\n") != std::string::npos)
        return true;
    return false;
}

void Server::handleClientMessage(int clientSocket) {
    int bytesRead;
    Client *client = getClientByFd(clientSocket);
    if(!getClientMessage(clientSocket, bytesRead))
        return;
    std::vector<std::string> commands;
    commands = splitCommands(client->messageBuffer);
    client->messageBuffer.clear();
    epollState(epollfd, clientSocket, EPOLLOUT);
    std::map<int, Client*>::iterator it = clients.find(clientSocket);
    if (it != clients.end())
        for (size_t i = 0; i < commands.size(); ++i) {
            parseMessage(commands[i], it);
        }
    else
        std::cout << "Client not found for socket " << clientSocket << std::endl;
}

void Server::parseMessage(const std::string &message, std::map<int, Client*>::iterator it) {
    std::stringstream iss(message);
    std::string command;
    iss >> command;
    if (command == "NICK") {            //finish
        hNickCmd(iss, it);
    } else if (command == "USER") {     //finish
        hUserCmd(iss, it);
    } else if (command == "JOIN") {     //finish
        hJoinCmd(iss, it);
    }else if(command == "SF") {
        hSFCmd(iss, it);
    }else if(command == "SFA") {
        hSFACmd(iss, it);
    }else if (command == "PART") {     //finish
        hPartCmd(iss, it);
    } else if (command == "PRIVMSG") {  //finish
        hPrivMsgCmd(iss, it);
    } else if (command == "KICK") {     //finish
        hKickCmd(iss, it);
    } else if (command == "INVITE") {   //finish
        hInviteCmd(iss, it);
    } else if (command == "TOPIC") {    //finish corrigir -t do op
        hTopicCmd(iss, it);
    }else if(command == "NOTICE"){
        hNoticeCmd(iss, it);
    } else if (command == "MODE") {     //finish
        hModeCmd(iss, it);
    } else if (command == "WHO") {      //
        hWhoCmd(iss, it);
    } else if (command == "PASS") {     //finish
        hPassCmd(iss, it);
    } else if (command == "QUIT") {     //finish finalisar ctrl c
        hQuitCmd(iss, it);
        return ;
    } else if(command == "BOT") {      //alterar cmds e names
        hBotCmd(iss, it);
    } else {
        return;
    }
    epollState(epollfd, it->first, EPOLLIN);
}

// Bot join #a 
// Bot leave #a
// Bot Ola
// Bot time
// Bot send ./bla/fdx.txt manuel
// Bot gg