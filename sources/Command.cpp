#include <iostream>
#include <string>
#include <unistd.h>
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/rpl.hpp"

void	Server::join(int clientFd){
	bool tkfd = false;
	Server	&server = Server::getInstance();
	if (server._arrayParams.params.empty())
	{
		send(clientFd, ERR_NEEDMOREPARAMS(server.getUser(clientFd).getNickname(), "JOIN").c_str(), ERR_NEEDMOREPARAMS(server.getUser(clientFd).getNickname(), "JOIN").size(), 0);
		return;
	}
	if (server._arrayParams.params[0][0] == '#')
	{
		server._arrayParams.params[0].erase(0, 1);
	}
	size_t pos = server._arrayParams.params[0].find("\r\n");
	if (pos != std::string::npos)
		server._arrayParams.params[0] = server._arrayParams.params[0].substr(0, pos);
	if (isChannel(server._arrayParams.params[0])) {
		Channel &chan = getChannel(server._arrayParams.params[0]);
		if (chan.isInvOnly() == true) {
			if (server.getChannel(server._arrayParams.params[0]).isInvited(clientFd) == false) {
				send(clientFd, ERR_INVITEONLYCHAN(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).c_str(), ERR_INVITEONLYCHAN(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).size(), 0);
				return;
			}
		}
		if (chan.isLimitMode() == true) {
			if (chan.getUsers().size() == chan.getLimit()) {
				send(clientFd, ERR_CHANNELISFULL(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).c_str(), ERR_CHANNELISFULL(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).size(), 0);
				return;
			}
		}
		if (chan.isKeyMode() == true) {
			if (server._arrayParams.params.size() == 2) {
				if (server._arrayParams.params[1] != chan.getKey()) {
					send(clientFd, ERR_BADCHANNELKEY(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).c_str(), ERR_BADCHANNELKEY(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).size(), 0);
					return;
				}
			}
			else {
				send(clientFd, ERR_BADCHANNELKEY(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).c_str(), ERR_BADCHANNELKEY(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).size(), 0);
				return;
			}
		}
		if (chan.getUser(clientFd) != NULL) {
			return;
		}
		chan.addUser(server, clientFd);
		tkfd = true;
	}
	else {
		createChannel(server, clientFd, server._arrayParams.params[0]);
	}
	std::ostringstream oss;
	oss << clientFd;
	std::string fd = oss.str();

	//concatenate the nickname of the users in the channel
	std::string list_of_nicks = "";
	for (std::map<int, User*>::iterator it = server.getChannel(server._arrayParams.params[0]).getUsers().begin(); it != server.getChannel(server._arrayParams.params[0]).getUsers().end(); it++) {
		if (server.getChannel(server._arrayParams.params[0]).isOperator(it->second->getFd()))
			list_of_nicks += "@" + it->second->getNickname() + " ";
		else
			list_of_nicks += it->second->getNickname() + " ";
	}
	send(clientFd, RPL_JOIN(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).c_str(), RPL_JOIN(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).size(), 0);
	send(clientFd, RPL_NAMREPLY(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName(), list_of_nicks).c_str(), RPL_NAMREPLY(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName(), list_of_nicks).size(), 0);
	send(clientFd, RPL_ENDOFNAMES(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).c_str(), RPL_ENDOFNAMES(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName()).size(), 0);
	if (tkfd == true)
		send(clientFd, RPL_TOPIC(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName(), server.getChannel(server._arrayParams.params[0]).getTopic()).c_str(), RPL_TOPIC(server.getUser(clientFd).getNickname(), server.getChannel(server._arrayParams.params[0]).getName(), server.getChannel(server._arrayParams.params[0]).getTopic()).size(), 0);
	server.getUser(clientFd).getMyChannels().push_back(server._arrayParams.params[0]);
}

void	Server::invite(std::string nickname, std::string channel, int clientFd) {
	Server	&server = Server::getInstance();
	unsigned int user_fd = server.getTargetUserFd(nickname);

	if (server.isUser(user_fd) == true) {
		if (channel[0] != '#') {
			send(clientFd, ERR_NOSUCHCHANNEL(server.getUser(clientFd).getNickname(), channel).c_str(), ERR_NOSUCHCHANNEL(server.getUser(clientFd).getNickname(), channel).size(), 0);
			return;
		}
		channel.erase(0, 1);
		size_t pos = channel.find("\r\n");
		if (pos != std::string::npos)
			channel = channel.substr(0, pos);
		if (server.isChannel(channel) == true) {
			User user = server.getUser(user_fd);
			user.getMyChannels().push_back(channel);
			server.getChannel(channel).getInvited().insert(std::make_pair(user_fd, &user));
			send(clientFd, RPL_INVITING(server.getUser(clientFd).getNickname(), server.getUser(user_fd).getNickname(), channel).c_str(), RPL_INVITING(server.getUser(clientFd).getNickname(), server.getUser(user_fd).getNickname(), channel).size(), 0);
		}
		else
			send(clientFd, ERR_NOSUCHCHANNEL(server.getUser(clientFd).getNickname(), channel).c_str(), ERR_NOSUCHCHANNEL(server.getUser(clientFd).getNickname(), channel).size(), 0);
	}
	else
		send(clientFd, ERR_NOSUCHNICK(server.getUser(clientFd).getNickname(), nickname).c_str(), ERR_NOSUCHNICK(server.getUser(clientFd).getNickname(), nickname).size(), 0);
}

void	Server::quit(int clientFd){

	Server	&server = Server::getInstance();

	close(clientFd);
	epoll_ctl(server._epollFd, EPOLL_CTL_DEL, clientFd, NULL);
	deleteUser(clientFd);
}
