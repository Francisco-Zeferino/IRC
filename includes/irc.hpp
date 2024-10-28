#ifndef IRC_HPP
#define IRC_HPP

#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "numeric_responses.hpp"
#include "bot.hpp"

#include <poll.h>
#include <fcntl.h> 
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstring>
#include <csignal>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <map>
#include <vector>
#include <sys/epoll.h>
#include <errno.h>
#include "bot.hpp"
#include <ctime>
#include <iomanip>

#endif