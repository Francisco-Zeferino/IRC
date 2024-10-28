// #include "client.hpp"
// #include "server.hpp"
// #include "channel.hpp"
#include "bot.hpp"

Bot::Bot(int socket) {
    _socket = socket;
    setNick("tony");
    setUser("Marvin");
}

int Server::createBotSocket() {
    int botSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (botSocket < 0) {
        std::cerr << "Failed to create bot socket\n";
        return -1;
    }


    return botSocket;
}

Bot *Server::startBot(Channel *channel) {
    (void)channel;
    int botSocket = createBotSocket();  // Create socket for bot connection
    if (botSocket == -1) {
        std::cerr << "Bot socket creation failed\n";
        return NULL;
    }

    // std::cout << "Bot TEST" << bot->getSocket() << std::endl;
    // Register bot's socket 
    struct epoll_event botevent;
    botevent.data.fd = botSocket;
    botevent.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, botSocket, &botevent) == -1) {
        std::cerr << "Failed to add bot to epoll\n";
        return NULL;
    }
    return new Bot(botSocket); 
}


// void Bot::valitadeBotMsg(const std::string &message) {
//     if (message.find("hello") != std::string::npos || message.find("ola") != std::string::npos) {
//         greetBotMsg();
//     }
//     else if (message == "!time") {
//         timeBotMsg(channel);
//     }
// }

// void Bot::greetBotMsg(Channel *channel) {
//     std::string reply = "Wazzzaaaaaaaaaaaaa! How can i help you?";
//     sendBotMsg(channel, reply);
// }

//  void Bot::timeBotMsg(Channel* channel) {
//     std::time_t currentTime = std::time(NULL);
//     std::ostringstream timeStream;
//     timeStream << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");

//     std::string reply = "Sao horas de comer pao: " + timeStream.str();
//     sendBotMsg(channel, reply);
// }

// void Bot::sendBotMsg(Channel* channel, const std::string& message) {
//     std::string fullMessage = ":" + getNick() + "!" + getUser() + "@localhost PRIVMSG " + channel->name + " :" + message + "\r\n";
//     channel->sendMsg(fullMessage, getSocket());
// }

// // void Server::addBot2Channel(const std::string& botNick, const std::string& botUser, const std::string& channelName) {
// //     Channel* channel = findChannel(channelName);
// //     if (channel == nullptr) {
// //         channel = createChannel(channelName);
// //     }

// //     int botSocket = createBotSocket();
// //     Bot* bot = new Bot(botSocket, botNick, botUser);
// //     clients[botSocket] = bot;
// //     channel->addClient(bot, true);

// //     std::string joinMessage = ":" + bot->getNick() + "!" + bot->getUser() + "@localhost JOIN " + channelName + "\r\n";
// //     channel->notifyAllInChannel(channel, joinMessage);

// //     std::cout << "Bot " << botNick << " joined channel " << channelName << "\n";
// // }

// // int Server::createBotSocket() {
// //     int botSocket = socket(AF_INET, SOCK_STREAM, 0);
// //     if (botSocket < 0) {
// //         std::cerr << "Error creating bot socket\n";
// //         return -1;
// //     }

// //     // Setting up the bot's socket address (localhost)
// //     sockaddr_in botAddress{};
// //     botAddress.sin_family = AF_INET;
// //     botAddress.sin_port = htons(0); // Use an ephemeral port for bot
// //     botAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

// //     if (bind(botSocket, (struct sockaddr*)&botAddress, sizeof(botAddress)) < 0) {
// //         std::cerr << "Error binding bot socket\n";
// //         close(botSocket);
// //         return -1;
// //     }

// //     // Set non-blocking mode for bot socket if needed
// //     int flags = fcntl(botSocket, F_GETFL, 0);
// //     if (flags == -1) {
// //         std::cerr << "Error getting bot socket flags\n";
// //         close(botSocket);
// //         return -1;
// //     }
// //     if (fcntl(botSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
// //         std::cerr << "Error setting bot socket to non-blocking\n";
// //         close(botSocket);
// //         return -1;
// //     }

// //     return botSocket;
// // }
