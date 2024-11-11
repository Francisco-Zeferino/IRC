#include "server.hpp"
#include "client.hpp"

void Server::sendFile(std::fstream &cFile, int socket, std::string fileName){
    std::stringstream fileStringStream;
    std::string rawFile;
    cFile.open(fileName.c_str());
    if(!cFile.is_open()){
        std::cout << "Error opening file" << std::endl;
        cFile.close();
        return ;
    }
    fileStringStream << cFile.rdbuf();
    rawFile = fileStringStream.str();
    sendMsgServ(rawFile, socket);
}

void readFile(int socket, std::string fileName){
    fileName.append("123");
    std::ofstream rvFile(fileName.c_str(), std::ios::binary);
    char buffer[1024] = {};
    read(socket, buffer, 1024);
    rvFile.write("buffer", 1024);
    rvFile.close();
}

void Server::hSFCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string targetUser, fileName;
    iss >> targetUser >> fileName;
    if(targetUser.empty() || fileName.empty()) {
        sendMsgServ(ERR_NEEDMOREPARAMS(it->second->getNick(), "SF"), it->first);
        return;
    }
    Client *targetClient = getClient(targetUser);
    if(!targetClient) {
        sendMsgServ(ERR_NOSUCHNICK(it->second->getNick(), targetUser), it->first);
        return;
    }
    targetClient->filePool.insert(std::make_pair(it->second, fileName));
    std::string message = RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), targetUser, " :File transfer request sent");
    sendMsgServ(message, targetClient->getSocket());
    message = RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), it->second->getNick(), " :Command SFA to accept or ignore");
    sendMsgServ(message, targetClient->getSocket());
}

void Server::hSFACmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string targetUser;
    iss >> targetUser;
    std::cout << targetUser << std::endl;
    if(targetUser.empty()) {
        sendMsgServ(ERR_NEEDMOREPARAMS(it->second->getNick(), "SFA"), it->first);
        return;
    }
    Client *targetClient = getClient(targetUser);
    if(!targetClient) {
        sendMsgServ(ERR_NOSUCHNICK(it->second->getNick(), targetUser), it->first);
        return;
    }
    std::map<Client *, std::string>::iterator fileIt = it->second->filePool.find(targetClient);
    if(fileIt == it->second->filePool.end()) {
        sendMsgServ(ERR_NOSUCHNICK(it->second->getNick(), targetUser), it->first);
        return;
    }
    std::string message = RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), targetUser, " :File transfer request accepted");
    sendMsgServ(message, targetClient->getSocket());
    message = RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), it->second->getNick(), " :File transfer request accepted");
    sendMsgServ(message, it->first);
    std::fstream sfile;
    sendFile(sfile, it->first, fileIt->second);
    readFile(it->first, "ola");
    targetClient->filePool.erase(fileIt);
}

