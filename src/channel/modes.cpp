#include "server.hpp"
#include "channel.hpp"
#include "client.hpp"

void Server::hModeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, mode;
    iss >> channelName >> mode;

    if(isClientAuthenticated(it->second) == false){
        std::cout << "Not authenticated to server." << std::endl;
        return ;
    }
    Channel* channel = findChannel(channelName); //alt
    if (!channel) {
        std::cout << "Channel not found: " << channelName << "\n";
        return;
    }

    if(mode.empty()){
        channel->sendMsg(ERR_UNKNOWNMODE(it->second->getNick(), mode), it->first);
        return;
    }

    if (!channel->isAdmin(it->second)) {
        channel->sendMsg(ERR_CHANOPRIVSNEEDED(it->second->getNick(), channelName), it->first);
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
            aPasswordMode(iss, modeSign == '+', requester);
            break;
        case 'l':
            aUserLimitMode(iss, modeSign == '+');
            break;
        case 't':
            aTopicMode(modeSign == '+');
            break;
        default:
            sendMsg(ERR_UNKNOWNMODE(requester->getNick(), mode), requester->getSocket());
            break;
    }
}

void Channel::aOperatorMode(std::stringstream &iss, bool addOperator, Client *requester) {
    std::string targetNick;
    iss >> targetNick;

    Client* targetClient = NULL; ///corigir esta verificacao + if
    for (std::map<Client*, bool>::iterator it = admins.begin(); it != admins.end(); ++it) {
        if (it->first->getNick() == targetNick) {
            targetClient = it->first;
            break;
        }
    }
    if (!targetClient) {
        sendMsg(ERR_NOOPERHOST(requester->getNick(), name), requester->getSocket());
        return;
    }
    // Defense for self removal op
    if (targetClient == requester && !addOperator) {
        std::string message = ":localhost 481 " + requester->getNick() + " " + name + " :\00304You cannot remove your own operator status!\00304\r\n";
        sendMsg(message, requester->getSocket());
        return;
    }
    if (admins[targetClient] == addOperator) {
        std::string message = addOperator 
            ? ERR_USERISALREADYOP(requester->getNick(), name) 
            : ERR_USERISNOTOP(requester->getNick(), name);
        sendMsg(message, requester->getSocket());
        return;
    }
    admins[targetClient] = addOperator;
    std::string modeChangeMsg = ":" + requester->getNick() + " MODE " + name + (addOperator ? " +o " : " -o ") + targetNick + "\r\n";
    notifyAllInChannel(this, modeChangeMsg);
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
    notifyAllInChannel(this, modeChangeMsg);
}

void Channel::aPasswordMode(std::stringstream &iss, bool enable, Client *requester) {
    if (enable) {
        std::string newPassword;
        iss >> newPassword;

        if (newPassword.empty()) {
            std::string errorMsg = ERR_PASSWDMISMATCH();
            notifyAllInChannel(this, errorMsg);
            return;
        }
        this->password = newPassword;
        mode += 'k';

        std::cout << "Password set for channel " << name << "\n";
        std::string message = ":" + requester->getNick() + "!" + requester->getUser() + "@localhost MODE " + name + " +k " + newPassword + "\r\n";
        notifyAllInChannel(this, message);
    } 
    else {
        this->password.clear();
        mode.erase(mode.find('k'), 1);

        std::cout << "Password removed for channel " << name << "\n";
        std::string message = ":" + requester->getNick() + "!" + requester->getUser() + "@localhost MODE " + name + " -k\r\n";
        notifyAllInChannel(this, message);
    }
}

bool isValid(std::string toCheck){
    for(size_t i = 0; i < toCheck.length(); i++){
        if(isdigit(toCheck[i]) == 0)
            return false;
    }
    return true;
}

void Channel::aUserLimitMode(std::stringstream &iss, bool enable) {
    if (enable) {
        std::string newLimit;
        iss >> newLimit;
        if(newLimit.empty())
            return ;
        if (isValid(newLimit)) {
            userslimit = atoi(newLimit.c_str());
            mode = "l";
            std::cout << "User limit set to " << userslimit << " for channel: " << name << std::endl;
            notifyAllInChannel(this, ":localhost " + iss.str() + "\r\n");
        } else {
            std::cout << "Invalid user limit provided.\n";
            return;
        }
    } else {
        if (hasMode('l')) {
            mode.erase(mode.find('l'), 1);
            userslimit = 0;
            std::cout << "User limit mode disabled for channel " << name << "\n";
            notifyAllInChannel(this, ":localhost MODE " + name + " -l\r\n");
        } else {
            std::cout << "User limit mode not set, cannot disable.\n";
            return;
        }
    }

    // std::string modeChangeMsg = ":localhost MODE " + name + << " (userslimit) : " -l") + "\r\n";
    // notifyAllInChannel(this, modeChangeMsg);
}

void Channel::aTopicMode(bool enable) {
    if (enable) {
        if (!hasMode('t')) {
            mode += 't';
            std::cout << "Topic protection enabled for channel " << name << "\n";
        } else {
            std::cout << "Topic protection is already enabled for channel " << name << "\n";
        }
    } else {
        if (hasMode('t')) {
            mode.erase(mode.find('t'), 1);
            std::cout << "Topic protection disabled for channel " << name << "\n";
        } else {
            std::cout << "Topic protection is already disabled for channel " << name << "\n";
        }
    }
    std::string modeChangeMsg = ":localhost MODE " + name + (enable ? " +t" : " -t") + "\r\n";
    notifyAllInChannel(this, modeChangeMsg);
}
