#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

void Server::hPassCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string receivedPassword;
    iss >> receivedPassword;

    if (this->password != receivedPassword) {
        // channel->sendMsg(ERR_PASSWDMISMATCH(), it->first);
        close(it->first);
        std::cout << "Client disconnected due to incorrect password/n";
    } else {
        std::cout << "Client " << it->second->getNick() << " authenticated with correct server password\n";
    }
    
}