#include "server.hpp"
#include "client.hpp"

size_t getFileSize(std::fstream& file) {
    file.seekg(0, std::ios::end); // Move to the end of the file
    size_t fileSize = file.tellg(); // Get the current position (file size)
    file.seekg(0, std::ios::beg); // Move back to the beginning of the file
    return fileSize;
}

void setSender(std::string rawData){
    struct sockaddr_in senderAddr;
    int sender = socket(AF_INET, SOCK_STREAM, 0);
    if(sender == -1) {
        std::cerr << "Error creating sender socket" << std::endl;
        exit(1);
    }
    senderAddr.sin_family = AF_INET;
    senderAddr.sin_port = htons(9000);
    senderAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(connect(sender, (struct sockaddr *)&senderAddr, sizeof(senderAddr)) == -1) {
        std::cerr << "Error connecting to server" << std::endl;
        exit(1);
    }
    int size = send(sender, rawData.c_str(), rawData.size(), 0);
    if(size == -1) {
        std::cerr << "Error sending data" << std::endl;
        exit(1);
    }
    close(sender);
}

void Server::setReceiver(size_t fileSize){
    int incmoningConnection, bytesReceived;
    struct sockaddr_in receiverAddr, incommingConnectionAddr;
    char buffer[fileSize];
    size_t incommingConnectionAddrSize = sizeof(incommingConnectionAddr);
    int receiver = socket(AF_INET, SOCK_STREAM, 0);
    if(receiver == -1) {
        std::cerr << "Error creating receiver socket" << std::endl;
        exit(1);
    }
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(9000);
    receiverAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(receiver, (struct sockaddr *)&receiverAddr, sizeof(receiverAddr)) == -1) {
        std::cerr << "Error binding receiver socket" << std::endl;
        exit(1);
    }
    std::cout << "File opened1" << std::endl;
    if(listen(receiver, 1) == -1) {
        std::cerr << "Error listening on receiver socket" << std::endl;
        exit(1);
    }
    std::cout << "File opened2" << std::endl;
    setSender(raw.str());
    if((incmoningConnection = accept(receiver, (struct sockaddr *)&incommingConnectionAddr, (socklen_t *)&incommingConnectionAddrSize)) == -1) {
        std::cout << "Error accepting connection" << std::endl;
        exit(1);
    }
    std::cout << "File opened3" << std::endl;
    std::ofstream newFile;
    newFile.open("newFile", std::ios::out | std::ios::binary);
    while((bytesReceived = recv(incmoningConnection, buffer, fileSize, 0)) > 0) {
        newFile.write(buffer, bytesReceived);
    }
    if(bytesReceived == -1) {
        std::cerr << "Error receiving data" << std::endl;
        exit(1);
    }
    if(bytesReceived == 0) {
        std::cout << "File received" << std::endl;
    }
    newFile.close();
    close(incmoningConnection);
    close(receiver);
    std::cout << "File received" << std::endl;
}

void Server::startDcc(const std::string fileName){
    std::fstream sFile;
    sFile.open(fileName.c_str(), std::ios::in | std::ios::binary);
    if(!sFile.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    raw << sFile.rdbuf();
    setReceiver(getFileSize(sFile));
    sFile.close();
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
    startDcc(fileIt->second);
    targetClient->filePool.erase(fileIt);
}