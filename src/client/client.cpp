#include "client.hpp"
#include "server.hpp"
#include "channel.hpp"

Client::Client() {}

Client::Client(int socket) : isAuthenticated(false), passwordAuthenticated(false), _socket(socket) {}

int Client::getSocket() const {
    return _socket;
}

std::string Client::getNick() const {
    return _nick;
}

std::string Client::getUser() const {
    return _user;
}

void Client::setNick(const std::string &nickname) {
    _nick = nickname;
}

void Client::setUser(const std::string &username) {
    _user = username;
}
