//////////////////// REPLY MESSAGES ///////////////
#define user_info(nick, user) (":" + nick + "!" + user + "@localhost")
//
#define RPL_WELCOME(user_info, nick)(user_info + " 001 " + nick + " :\00303Welcome to the localhost Network\00303, " + nick + "\r\n")
//
#define RPL_INVITING(user_info, target, channel) (user_info + " INVITING " + target + " " + channel + "\r\n")
//
#define RPL_INVITE(user_info, target, channel) (user_info + " INVITE " + target + " " + channel + "\r\n")
#define RPL_PRIVMSG(user_info, target, message) (user_info + " PRIVMSG " + target + message + "\r\n")
// Successful operator promotion
#define RPL_OPGIVEN(nick, target, channel) (":localhost 324 " + nick + " " + target + " " + channel + " :\00303" + target + " is now a channel operator\00303\r\n")
// Successful operator demotion
#define RPL_OPREMOVED(nick, target, channel) (":localhost 324 " + nick + " " + target + " " + channel + " :\00303" + target + " is no longer a channel operator\00303\r\n")
// Reply for a channel with a topic set
#define RPL_TOPIC(nick, channel, topic) (":localhost 332 " + nick + " " + channel + " :\00308" + topic + "\r\n")
// Reply for a channel without a topic set
#define RPL_NOTOPIC(nick, channel) (":" + nick + "!" + nick + "@localhost 331 " + nick + " " + channel + " :\00308No topic is set\r\n")
// 
#define RPL_YOUREOPER(nick, channel) (":" + nick + "!" + nick + "@localhost 381 " + nick + " " + channel + " :\00303You are now an IRC operator\00303\r\n")
// Communicates the current mode of the channel to the user
#define RPL_CHANNELMODEIS(nick, channel, mode) (":localhost 324 " + nick + " " + channel + " :" + mode + "\r\n")


///////////////////// ERRORS MESSAGES //////////////////////
// 
#define ERR_NOOPERHOST(nick, channel)(":localhost 491 " + nick + " " + channel + " :\00304Given user not in channel\00304\r\n")
// 
#define ERR_PASSWDMISMATCH()(":localhost 464 : \00304Incorrect password\00304\r\n")
// 
#define ERR_CHANNELISFULL(nick, channel)(":localhost 471 " + nick + " " + channel + " :\00304Can't join channel, max user reached!\00304\r\n")
// Channel operator privilege required
#define ERR_CHANOPRIVSNEEDED(nick, channel) (":localhost 482 " + nick + " " + channel + " :\00304You're not a channel operator\00304\r\n")
// No such channel
#define ERR_NOSUCHCHANNEL(nick, channel) (":localhost 403 " + nick + " " + channel + " :\00304No such channel\00304\r\n")
// User not in channel (used when trying to kick or promote a user not in the channel)
#define ERR_USERNOTINCHANNEL(nick, channel) (":localhost 441 " + nick + " " + channel + " :\00304User is not in the channel\00304\r\n")
// User not in the channel (used when user tries to leave a channel they are not in)
#define ERR_NOTONCHANNEL(nick, channel) (":localhost 442 " + nick + " " + channel + " :\00304You're not on that channel\00304\r\n")
// Uknow mode
#define ERR_UNKNOWNMODE(nick, mode) (":localhost 472 " + nick + " :Unknown mode " + mode + "\r\n")
// 
#define ERR_NOSUCHNICK(nick, target) (":localhost 401 " + nick + " " + target + " :\00304No such nick/channel\00304\r\n")
// Invite only channel
#define ERR_INVITEONLYCHAN(nick, channel)(":localhost 473 " + nick + " " + channel + " :Cannot join channel (+i)\r\n")
//  user provides an incorrect password to join a password-protected channel.
#define ERR_BADCHANNELKEY(nick, channel) (":localhost 475 " + nick + " " + channel + " :\00304Incorrect channel key (password)\00304\r\n")
// indicating that the user is already an operator for the specified channel. 
#define ERR_USERISALREADYOP(nick, channel) (":localhost 441 " + nick + " " + channel + " :\00304User is already an operator!\00304\r\n")
// 
#define ERR_USERISNOTOP(nick, channel) (":localhost 442 " + nick + " " + channel + " :\00304User is not an operator!\00304\r\n")
// Error when the nickname is already in use
#define ERR_NICKNAMEINUSE(nick, newNick) (":localhost 433 " + nick + " " + newNick + " :Nickname is already in use\r\n")
// Error when the nickname is invalid or contains invalid characters
#define ERR_ERRONEUSNICKNAME(nick, newNick) (":localhost 432 " + nick + " " + newNick + " :Erroneous nickname\r\n")
//Indicates a missing parameter for a command
#define ERR_NEEDMOREPARAMS(nick, command) (":localhost 461 " + nick + " " + command + " :Not enough parameters\r\n")
