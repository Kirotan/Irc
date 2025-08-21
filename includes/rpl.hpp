#ifndef NUMERICAL_REPLIES_HPP
#define NUMERICAL_REPLIES_HPP

#include <string>
#include <algorithm>


# define user_id(nickname, username) (":" + nickname + "!" + username + "@server_pika")

# define RPL_WELCOME(user_id, nickname) (":server_pika 001 " + nickname + " :Welcome to the Internet Relay Network " + user_id + "\r\n")
# define RPL_YOURHOST(client, servername, version) (":server_pika 002 " + client + " :Your host is " + servername + " (server_pika), running version " + version + "\r\n")
# define RPL_CREATED(client, datetime) (":server_pika 003 " + client + " :This server was created " + datetime + "\r\n")
# define RPL_MYINFO(client, servername, version, user_modes, chan_modes, chan_param_modes) (":server_pika 004 " + client + " " + servername + " " + version + " " + user_modes + " " + chan_modes + " " + chan_param_modes + "\r\n")
# define RPL_ISUPPORT(client, tokens) (":server_pika 005 " + client + " " + tokens " :are supported by this server\r\n")

# define ERR_UNKNOWNCOMMAND(client, command) (":server_pika 421 " + client + " " + command + " :Unknown command\r\n")

// INVITE
# define ERR_NEEDMOREPARAMS(client, command) (":server_pika 461 " + client + " " + command + " :Not enough parameters.\r\n")
# define ERR_INVITEONLYCHAN(client, channel) (":server_pika 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
# define ERR_NOSUCHCHANNEL(client, channel) (":server_pika 403 " + client + " #" + channel + " :No such channel\r\n")
# define ERR_NOTONCHANNEL(client, channel) (":server_pika 442 " + client + " #" + channel + " :The user is not on this channel.\r\n")
# define ERR_USERONCHANNEL(client, nick, channel) (":server_pika 443 " + client + " " + nick + " #" + channel + " :Is already on channel\r\n")
# define RPL_INVITING(client, nick, channel) (":server_pika 341 " + client + " " + nick + " #" + channel + "\r\n")
# define RPL_INVITE(user_id, invited, channel) (user_id + " INVITE " + invited + " #" + channel + "\r\n")

// JOIN
# define RPL_JOIN(user_id, channel) (":" + user_id + " JOIN :#" +  channel + "\r\n")
# define ERR_BANNEDFROMCHAN(client, channel) ("474 " + client + " #" + channel + " :Cannot join channel (+b)\r\n")
# define ERR_BADCHANNELKEY(client, channel) ("475 " + client + " #" + channel + " :Cannot join channel (+k)\r\n")

// KICK
# define ERR_USERNOTINCHANNEL(client, nickname, channel) ("441 " + client + " " + nickname + " #" + channel + " :They aren't on that channel\r\n")
# define RPL_KICK(user_id, channel, kicked) (user_id + " KICK #" + channel + " " + kicked + "\r\n")

// KICK & channel mode
# define ERR_CHANOPRIVSNEEDED(client, channel) (":server_pika 482 " + client + " #" + channel + " :You're not channel operator\r\n")
// KICK & PRIVMSG
# define ERR_NOSUCHNICK(client, target) ("401 " + client + " " + target + " :No such nick/channel\r\n")

// MODE
/* user mode */
#define MODE_USERMSG(client, mode) (":" + client + " MODE " + client + " :" + mode + "\r\n")
#define ERR_UMODEUNKNOWNFLAG(client) (":server_pika 501 " + client + " :Unknown MODE flag\r\n")
#define ERR_USERSDONTMATCH(client) ("502 " + client + " :Cant change mode for other users\r\n")
#define RPL_UMODEIS(client, mode) (":server_pika 221 " + client + " " + mode + "\r\n")
/* channel mode */
#define MODE_CHANNELMSG(channel, mode) (":server_pika MODE #" + channel + " " + mode + "\r\n")
#define MODE_CHANNELMSGWITHPARAM(channel, mode, param) (":server_pika MODE #" + channel + " " + mode + " " + param + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, mode) (":server_pika 324 " + client + " #" + channel + " " + mode + "\r\n")
#define RPL_CHANNELMODEISWITHKEY(client, channel, mode, password) (":server_pika 324 " + client + " #" + channel + " " + mode + " " + password + "\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel) ("404 " + client + " #" + channel + " :Cannot send to channel\r\n")
#define ERR_CHANNELISFULL(client, channel) ("471 " + client + " #" + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVALIDMODEPARAM(client, channel, mode, password) ("696 " + client + " #" + channel + " " + mode + " " + password + " : password must only contained alphabetic character\r\n")
#define RPL_ADDVOICE(nickname, username, channel, mode, param) (":" + nickname + "!" + username + "@server_pika MODE #" + channel + " " + mode + " " + param + "\r\n")

// MOTD
#define ERR_NOSUCHSERVER(client, servername) (":server_pika 402 " + client + " " + servername + " :No such server\r\n")
#define ERR_NOMOTD(client) (":server_pika 422 " + client + " :MOTD File is missing\r\n")
#define RPL_MOTDSTART(client, servername) (":server_pika 375 " + client + " :- " + servername + " Message of the day - \r\n")
#define RPL_MOTD(client, motd_line) (":server_pika 372 " + client + " :" + motd_line + "\r\n")
#define RPL_ENDOFMOTD(client) (":server_pika 376 " + client + " :End of /MOTD command.\r\n")


// NAMES
# define RPL_NAMREPLY(client, channel, list_of_nicks) (":server_pika 353 " + client + " = #" + channel + " :" + list_of_nicks + "\r\n")
# define RPL_ENDOFNAMES(client, channel) (":server_pika 366 " + client + " #" + channel + " :End of /NAMES list.\r\n")

// NICK
# define ERR_NONICKNAMEGIVEN(client) (":server_pika 431 " + client + " :There is no nickname.\r\n")
# define ERR_ERRONEUSNICKNAME(client, nickname) (":server_pika 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
# define ERR_NICKNAMEINUSE(client, nickname) (":server_pika 433 " + client + " " + nickname + " :Nickname is already in use.\r\n")
# define RPL_NICK(oclient, uclient, client) (":" + oclient + "!" + uclient + "@server_pika NICK " +  client + "\r\n")

// NOTICE
# define RPL_NOTICE(nick, username, target, message) (":" + nick + "!" + username + "@server_pika NOTICE " + target + " " + message + "\r\n")

// OPER
# define ERR_NOOPERHOST(client) ("491 " + client + " :No O-lines for your host\r\n")
# define RPL_YOUREOPER(client) ("381 " + client + " :You are now an IRC operator\r\n")

// PART
# define RPL_PART(user_id, user_name, channel) (":" + user_id + "!" + user_name + " PART #" + channel + "\r\n")

// PASS
# define ERR_PASSWDMISMATCH(client) (":server_pika 464 " + client + " :Password incorrect.\r\n")

// PING
# define RPL_PONG(user_id, token) (":" + user_id + " :PONG " + token + "\r\n")

// QUIT
# define RPL_QUIT(user_id, reason) (user_id + " QUIT :Quit: " + reason + "\r\n")
# define RPL_ERROR(user_id, reason) (user_id + " ERROR :" + reason + "\r\n")

// PRIVMSG
# define ERR_NORECIPIENT(client) ("411 " + client + " :No recipient given PRIVMSG\r\n")
# define ERR_NOTEXTTOSEND(client) ("412 " + client + " :No text to send\r\n")
# define RPL_PRIVMSG(nick, target, message) (":" + nick + " PRIVMSG " + target + " " + message + "\r\n")

// TOPIC
# define RPL_TOPIC(client, channel, topic) (":server_pika 332 " + client + " #" + channel + " " + topic + "\r\n")
# define RPL_NOTOPIC(client, channel) (":server_pika 331 " + client + " #" + channel + " :No topic is set\r\n")

// USER
# define ERR_ALREADYREGISTERED(client) (":server_pika 462 " + client + " :You may not reregister.\r\n")

#endif
