#include "client.hpp"
#include "server.hpp"

Client::Client(int socket) : _socket(socket) {}

int Client::getSocket() const {
    return _socket;
}

std::string Client::getNick() const {
    return _nick;
}

std::string Client::getUser() const {
    return _user;
}

std::string Client::getChannel() const {
    return _channel;
}

void Client::setNick(const std::string &nickname) {
    _nick = nickname;
}

void Client::setUser(const std::string &username) {
    _user = username;
}

void Client::setChannel(const std::string &channelname) {
    _channel = channelname;
}

void Client::sendMsg(const std::string &msg, int clientSocket) const {
    send(clientSocket, msg.c_str(), msg.length(), 0);
}