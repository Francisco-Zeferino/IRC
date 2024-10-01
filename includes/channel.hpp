#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"
#include <vector>
#include <map>
#include <string>

class Channel {
    public :
        Channel();
        std::map<Client *, bool> channel;
        std::string name;
        std::string topic;
        std::string mode;

        void setTopic(std::string topic);
        void setMode(std::string mode);
        void setName(std::string name);
};

#endif