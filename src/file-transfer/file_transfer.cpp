#include "server.hpp"
#include "client.hpp"

size_t getFileSize(std::ifstream& file) {
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    return fileSize;
}

void setSender(std::string rawData){
    struct sockaddr_in senderAddr;
    int sender = socket(AF_INET, SOCK_STREAM, 0);
    if(sender == -1) {
        std::cerr << "Error creating sender socket" << std::endl;
        return ;
    }
    senderAddr.sin_family = AF_INET;
    senderAddr.sin_port = htons(9000);
    senderAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(connect(sender, (struct sockaddr *)&senderAddr, sizeof(senderAddr)) == -1) {
        std::cerr << "Error connecting to server" << std::endl;
        return ;
    }
    int size = send(sender, rawData.c_str(), rawData.size(), 0);
    if(size == -1) {
        std::cerr << "Error sending data" << std::endl;
        return ;
    }
    rawData.clear();
    close(sender);
}

void Server::setReceiver(size_t fileSize, std::string fileName){
    int incmoningConnection, bytesReceived;
    struct sockaddr_in receiverAddr, incommingConnectionAddr;
    char buffer[fileSize];
    size_t incommingConnectionAddrSize = sizeof(incommingConnectionAddr);
    int receiver = socket(AF_INET, SOCK_STREAM, 0);
    if(receiver == -1) {
        std::cerr << "Error creating receiver socket" << std::endl;
        return ;
    }
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(9000);
    receiverAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(receiver, (struct sockaddr *)&receiverAddr, sizeof(receiverAddr)) == -1) {
        std::cerr << "Error binding receiver socket" << std::endl;
        return ;
    }
    if(listen(receiver, 1) == -1) {
        std::cerr << "Error listening on receiver socket" << std::endl;
        return ;
    }
    setSender(raw.str());
    if((incmoningConnection = accept(receiver, (struct sockaddr *)&incommingConnectionAddr, (socklen_t *)&incommingConnectionAddrSize)) == -1) {
        std::cout << "Error accepting connection" << std::endl;
        return ;
    }
    std::ofstream newFile;
    newFile.open(fileName.c_str(), std::ios::out | std::ios::binary);
    while((bytesReceived = recv(incmoningConnection, buffer, fileSize, 0)) > 0) {
        newFile.write(buffer, bytesReceived);
    }
    if(bytesReceived == -1) {
        std::cerr << "Error receiving data" << std::endl;
        return ;
    }
    if(bytesReceived == 0) {
        std::cout << "File received" << std::endl;
    }
    newFile.close();
    close(incmoningConnection);
    close(receiver);
    std::cout << "File received" << std::endl;
}

void Server::startDcc(const std::string fileName, const std::string savedFileName) {
    std::ifstream sFile;
    sFile.open(fileName.c_str(), std::ios::in | std::ios::binary);
    if(!sFile.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    raw << sFile.rdbuf();
    setReceiver(getFileSize(sFile), savedFileName);
    sFile.close();
}

void Server::hSFCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string targetUser, fileName;
    iss >> targetUser >> fileName;
    if(isClientAuthenticated(it->second) == false){
        std::cout << "Not authenticated to server. Can't send a file" << std::endl;
        return ;
    }
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
    message = RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), it->second->getNick(), "File Transfer request received. File name is " + fileName);
    sendMsgServ(message, targetClient->getSocket());
    message = RPL_NOTICE(user_info(it->second->getNick(), it->second->getUser()), it->second->getNick(), " :Use /SFA <username> <filename to be saved> to accept the file transfer request");
    sendMsgServ(message, targetClient->getSocket());
}

void Server::hSFACmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string targetUser, savedFileName;
    iss >> targetUser >> savedFileName;
    if(isClientAuthenticated(it->second) == false){
        std::cout << "Not authenticated to server. Can't accept a file." << std::endl;
        return ;
    }
    std::cout << targetUser << std::endl;
    if(targetUser.empty() || savedFileName.empty()) {
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
    startDcc(fileIt->second, savedFileName);
    it->second->filePool.erase(fileIt);
}