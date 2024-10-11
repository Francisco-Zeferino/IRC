#define user_info(nick, user) (":" + nick + "!" + user + "@localhost")

#define RPL_WELCOME(user_info, nick)(user_info + " 001 " + nick + " :\00303Welcome to the localhost Network\00303, " + nick + "\r\n")

#define RPL_INVITING(user_info, target, channel) (user_info + " INVITING " + target + " " + channel + "\r\n")
#define RPL_INVITE(user_info, target, channel) (user_info + " INVITE " + target + " " + channel + "\r\n")

//mode operator
#define RPL_YOUREOPER(user, channel) (":localhost 381 " + user + " " + channel + " :\00303You are now an IRC operator\00303\r\n")

#define ERR_NOOPERHOST(nick, channel)(":localhost 491 " + nick + " " + channel + " :\00304Given user not in channel\00304\r\n")
#define ERR_PASSWDMISMATCH()(":localhost 464 : \00304Incorrect password\00304\r\n")
#define ERR_CHANNELISFULL(nick, channel)(":localhost 471 " + nick + " " + channel + " :\00304Can't join channel, max user reached!\00304\r\n")