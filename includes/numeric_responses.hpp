#define user_info(nick, user) (":" + nick + "!" + user + "@localhost")

#define RPL_INVITING(user_info, target, channel) (user_info + " INVITING " + target + " " + channel + "\r\n")
#define RPL_INVITE(user_info, target, channel) (user_info + " INVITE " + target + " " + channel + "\r\n")

//mode operator
#define RPL_YOUREOPER(user, channel) (":localhost 381 " + user + " " + channel + " :\00303You are now an IRC operator\00303\r\n")
