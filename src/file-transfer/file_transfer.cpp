#include "server.hpp"
#include "client.hpp"

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
    //std::cout << targetClient->getNick() << std::endl;
    if(!targetClient) {
        std::cout << "Here" << std::endl;
        sendMsgServ(ERR_NOSUCHNICK(it->second->getNick(), targetUser), it->first);
        return;
    }
    std::map<Client *, std::string>::iterator fileIt = it->second->filePool.find(targetClient);
    if(fileIt == it->second->filePool.end()) {
        std::cout << "Here2" << std::endl;
        sendMsgServ(ERR_NOSUCHNICK(it->second->getNick(), targetUser), it->first);
        return;
    }
    std::string message = RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), targetUser, " :File transfer request accepted");
    sendMsgServ(message, targetClient->getSocket());
    message = RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), it->second->getNick(), " :File transfer request accepted");
    sendMsgServ(message, it->first);
    targetClient->filePool.erase(fileIt);

}