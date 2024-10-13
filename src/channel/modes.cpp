#include "server.hpp"
#include "channel.hpp"
#include "client.hpp"

void Server::hModeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, mode;
    iss >> channelName >> mode;

    Channel* channel = findOrCreateChannel(channelName);
    // if (!channel) {
    //     sendMsg(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
    //     return;
    // }

    if (!channel->isAdmin(it->second)) {
        sendMsg(ERR_CHANOPRIVSNEEDED(it->second->getNick(), channelName), it->first);
        return;
    }

    channel->applyMode(iss, mode, it->second);
}

void Channel::applyMode(std::stringstream &iss, const std::string mode, Client* requester) {
    if (mode.empty()) return;
    char modeSign = mode[0];
    char modeType = mode[1];

    switch (modeType) {
        case 'o':
            aOperatorMode(iss, modeSign == '+', requester);
            break;
        case 'i':
            aInviteOnlyMode(modeSign == '+');
            break;
        case 'k':
            aPasswordMode(iss, modeSign == '+');
            break;
        case 'l':
            aUserLimitMode(iss, modeSign == '+');
            break;
        case 't':
            // +t
            break;
        default:
            // sendMsg(ERR_UNKNOWNMODE(requester->getNick(), mode), requester->getSocket());
            break; 
    }
}

void Channel::aOperatorMode(std::stringstream &iss, bool addOperator, Client *requester) {
    (void)*requester;
    (void)addOperator;
    (void)&iss;
    // std::string targetNick;
    // iss >> targetNick;

    // Client* targetClient = requester->server->getClient(targetNick);
    // if (!targetClient) {
    //     sendMsg(ERR_NOOPERHOST(requester->getNick(), name), requester->getSocket());
    //     return;
    // }

    // if (admins[targetClient] == addOperator) {
    //     std::string message = addOperator ? ERR_USERISALREADYOP(requester->getNick(), name) : ERR_USERISNOTOP(requester->getNick(), name);
    //     sendMsg(message, requester->getSocket());
    //     return;
    // }

    // admins[targetClient] = addOperator;
    // std::string modeChangeMsg = ":localhost MODE " + name + (addOperator ? " +o " : " -o ") + targetNick + "\r\n";
    // notifyAllInChannel(this, modeChangeMsg);
}



void Channel::aInviteOnlyMode(bool enable) {
    if (enable && !hasMode('i')) {
        mode += 'i';
        std::cout << "Invite-only mode enabled for channel " << name << "\n";
    } else if (!enable && hasMode('i')) {
        mode.erase(mode.find('i'), 1);
        std::cout << "Invite-only mode disabled for channel " << name << "\n";
    }
    std::string modeChangeMsg = ":localhost MODE " + name + (enable ? " +i" : " -i") + "\r\n";
    // notifyAllInChannel(this, modeChangeMsg);
}

void Channel::aPasswordMode(std::stringstream &iss, bool enable) {
    if (enable) {
    std::string newPassword;
        iss >> newPassword;

        if (newPassword.empty()) {
            std::cout << "Error: Cannot set an empty password.\n";
            return;
        }

        this->password = newPassword;
        std::cout << "Password set for channel " << name << "\n";
        
        std::string message = ":localhost MODE " + name + " +k \r\n";
        // notifyAllInChannel(this, message);
    } 
    else {
        this->password.clear();
        std::cout << "Password removed for channel " << name << "\n";
        
        std::string message = ":localhost MODE " + name + " -k \r\n";
        // notifyAllInChannel(this, message);
    }
}

void Channel::aUserLimitMode(std::stringstream &iss, bool enable) {
    if (enable) {
        size_t newLimit;
        iss >> newLimit;

        if (newLimit > 0) {
            userslimit = newLimit;
            std::cout << "User limit set to " << userslimit << " for channel: " << name << std::endl;
        } else {
            std::cout << "Invalid user limit provided.\n";
            return; //add
        }
    } else {
        if (hasMode('l')) {
            mode.erase(mode.find('l'), 1);
            userslimit = 0;
            std::cout << "User limit mode disabled for channel " << name << "\n";
        } else {
            std::cout << "User limit mode not set, cannot disable.\n";
            return;
        }
    }

    std::cout << "Limits atm: " << userslimit << ".\n";
    // std::string modeChangeMsg = ":localhost MODE " + name + (enable ? " +l " + std::to_string(userslimit) : " -l") + "\r\n";
    // notifyAllInChannel(this, modeChangeMsg.str());
}
