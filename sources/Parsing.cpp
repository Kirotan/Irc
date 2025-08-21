#include <iostream>
#include <string>
#include <cstdlib>
#include "../includes/Server.hpp"
#include "../includes/display.hpp"
#include "../includes/rpl.hpp"

void	Server::whoParsing(std::vector<std::string> &params, unsigned int myfd) {
	Server	&server = Server::getInstance();
	if (params[0][0] == '#')
		params[0].erase(0, 1);
	size_t pos = params[0].find("\r\n");
	if (pos != std::string::npos)
		params[0] = params[0].substr(0, pos);
	if (isChannel(params[0])) {
		Channel &chan = getChannel(params[0]);
		if (chan.getUser(myfd) == NULL) {
			std::cout << "ERROR : User is not on that channel." << std::endl;
			return;
		}
		//concatenate the nickname of the users in the channel
		std::string list_of_nicks = "";
		for (std::map<int, User*>::iterator it = chan.getUsers().begin(); it != chan.getUsers().end(); it++) {
			if (chan.isOperator(it->second->getFd()))
				list_of_nicks += "@" + it->second->getNickname() + " ";
			else
				list_of_nicks += it->second->getNickname() + " ";
		}
		send(myfd, RPL_NAMREPLY(server.getUser(myfd).getNickname(), chan.getName(), list_of_nicks).c_str(), RPL_NAMREPLY(server.getUser(myfd).getNickname(), chan.getName(), list_of_nicks).size(), 0);
		send(myfd, RPL_ENDOFNAMES(server.getUser(myfd).getNickname(), chan.getName()).c_str(), RPL_ENDOFNAMES(server.getUser(myfd).getNickname(), chan.getName()).size(), 0);
	}
}

void	Server::parseTopic(Server &server, int clientFd) {
	if (server._arrayParams.params.size() == 0) {
		send(clientFd, ERR_NEEDMOREPARAMS(server.getUser(clientFd).getNickname(), "TOPIC").c_str(), ERR_NEEDMOREPARAMS(server.getUser(clientFd).getNickname(), "TOPIC").size(), 0);
		return;
	}
	if (server._arrayParams.params[0][0] == '#')
		server._arrayParams.params[0].erase(0, 1);
	size_t pos = server._arrayParams.params[0].find("\r\n");
	if (server._arrayParams.params.size() == 1 && pos != std::string::npos)
		server._arrayParams.params[0] = server._arrayParams.params[0].substr(0, pos);
	if (server.isChannel(server._arrayParams.params[0]) == false) {
		send(clientFd, ERR_NOSUCHNICK(server.getUser(clientFd).getNickname(), server._arrayParams.params[0]).c_str(), ERR_NOSUCHNICK(server.getUser(clientFd).getNickname(), server._arrayParams.params[0]).size(), 0);
		return;
	}
	if (server.getChannel(server._arrayParams.params[0]).getUser(clientFd) == NULL) {
		send(clientFd, ERR_NOTONCHANNEL(server.getUser(clientFd).getNickname(), server._arrayParams.params[0]).c_str(), ERR_NOTONCHANNEL(server.getUser(clientFd).getNickname(), server._arrayParams.params[0]).size(), 0);
		return;
	}
	if (isChannel(server._arrayParams.params[0])) {
		if (server._arrayParams.params.size() >= 2)
			getChannel(server._arrayParams.params[0]).setTopic(clientFd, server._arrayParams.params);
		else
			getChannel(server._arrayParams.params[0]).displayTopic(clientFd, server._arrayParams.params[0]);
	}
}

static void	checkParam(int argc, char **argv){

//Number argc verification
	if(argc != 3){
		std::cerr << "ircserv need 2 arguments : <port> <password>" << std::endl;
		exit (1);
	}

// Port verification
	unsigned short	i = 0;
	unsigned int	port;
	while(argv[1][i]){
		if(i > 4){
			std::cerr << "Port must be between 0 and 65535." << std::endl;
			exit (1);
		}
		if(isdigit(argv[1][i]) == 0){
			std::cerr << "Port must be made up of numbers only." << std::endl;
			exit (1);
		}
		i++;
	}

	port = static_cast<unsigned short>(atoi(argv[1]));
	if(port < 1024 || port > 49151){
		std::cerr << "Port must be between 1024 and 49151 for user applications.\
		\nPorts between 0 and 1023 ports are reserved for system services.\
		\nPorts between 49152 and 65535 are reserved for dynamic connections." << std::endl;
		exit(1);
	}
	else{
		Server &server = Server::getInstance();
		server.setPort(port);
	}

//Password verification
	std::string	password(argv[2]);

	Server &server = Server::getInstance();
	server.setPassword(password);
}

void	parsing(int argc, char **argv){checkParam(argc, argv);}
