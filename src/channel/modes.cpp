#include "server.hpp"
#include "channel.hpp"
#include "client.hpp"

void Server::hModeCmd(std::stringstream &iss, std::map<int, Client*>::iterator it) {
    std::string channelName, mode;
    iss >> channelName >> mode;

    Channel* channel = findOrCreateChannel(channelName);
    if (!channel) {
        sendMsg(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    if (!channel->isAdmin(it->second)) {
        sendMsg(ERR_CHANOPRIVSNEEDED(it->second->getNick(), channelName), it->first);
        return;
    }

    channel->applyMode(iss, mode, it->second);
}

void Channel::applyMode(std::stringstream &iss, const std::string mode, Client *requester) {
    (void)*requester;
    
    if (mode.empty()) return;
    char modeSign = mode[0];
    char modeType = mode[1];

    switch (modeType) {
        case 'o':
            // aOperatorMode(iss, modeSign == '+', requester);
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
        default:
            // sendMsg(ERR_UNKNOWNMODE(requester->getNick(), mode), requester->getSocket());
            break; 
    }
}

// void Channel::aOperatorMode(std::stringstream &iss, bool addOperator, Client *requester) {
//     void
//     std::string targetNick;
//     iss >> targetNick;

//     Client* targetClient = getClient(targetNick);  // Find the client in the server or channel

//     if (!targetClient) {
//         // If target user is not found
//         requester->sendMsg(ERR_NOSUCHNICK(requester->getNick(), targetNick), requester->getSocket());
//         return;
//     }

//     // Check if target client is in the channel
//     if (admins.find(targetClient) == admins.end()) {
//         std::string message = ERR_USERNOTINCHANNEL(requester->getNick(), targetNick, name);
//         requester->sendMsg(message, requester->getSocket());
//         return;
//     }

//     // Check if the target is already an operator
//     if (admins[targetClient] == addOperator) {
//         std::string message;
//         if (addOperator) {
//             message = ":localhost 491 " + requester->getNick() + " " + name + " :\00304User is already an operator!\00304\r\n";
//         } else {
//             message = ":localhost 491 " + requester->getNick() + " " + name + " :\00304User is not an operator!\00304\r\n";
//         }
//         // requester->sendMsg(message, requester->getSocket());
//         return;
//     }

//     // Grant or revoke operator status
//     admins[targetClient] = addOperator;
//     std::string message = RPL_YOUREOPER(targetNick, name);
//     // targetClient->sendMsg(message, targetClient->getSocket());

//     // Notify all users in the channel about the mode change
//     message = ":" + requester->getNick() + "!" + requester->getUser() + "@localhost MODE " + name + (addOperator ? " +o " : " -o ") + targetNick + "\r\n";
//     // notifyAllInChannel(this, message);
//     return;
// }



void Channel::aInviteOnlyMode(bool enable) {
    if (enable) {
        if (!hasMode('i')) {
            mode += 'i';
            std::cout << "Invite-only mode enabled for channel " << name << "\n";
        }
    } else {
        if (hasMode('i')) {
            mode.erase(mode.find('i'), 1);
            std::cout << "Invite-only mode disabled for channel " << name << "\n";
        }
    }
    // notifyAll(modeChangeMsg);
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
        }
    } else {
        mode.erase(mode.find('l'), 1);
        userslimit = 0; // funciona a 0?
        std::cout << "User limit mode disabled for channel " << name << "\n";
    }
    std::string modeChangeMsg = ":localhost MODE " + name + (enable ? " +l " + userslimit : " -l") + "\r\n";
    // notifyAll(modeChangeMsg);
}
