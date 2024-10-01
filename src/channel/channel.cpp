#include "channel.hpp"

Channel:: Channel(){}

void Channel::setTopic(std::string topic) {
    this->topic = topic;
}

void Channel::setMode(std::string mode) {
    this->mode = mode;
}

void Channel::setName(std::string name) {
    this->name = name;
}