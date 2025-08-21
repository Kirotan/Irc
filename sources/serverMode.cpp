#include "Server.hpp"
#include <string>
#include <../includes/rpl.hpp>

bool	Server::modeCmdParsing(std::vector<std::string> &params, unsigned int myfd) {
	Server	&server = Server::getInstance();
	std::string	&chanName = params[0];
	std::string	rplToSend = "";

	if(chanName == server.getUser(myfd).getNickname())
		return true;

	if (chanName[0] == '#') {
		chanName.erase(0, 1);
	} else {
		send(myfd, ERR_NOSUCHCHANNEL(server.getUser(myfd).getNickname(), chanName).c_str(), ERR_NOSUCHCHANNEL(server.getUser(myfd).getNickname(), chanName).size(), 0);
		return false;
	}

	size_t pos = chanName.find("\r\n");
	if (pos != std::string::npos)
		chanName = chanName.substr(0, pos);
	if (isChannel(chanName) == false) {
		send(myfd, ERR_NOSUCHCHANNEL(server.getUser(myfd).getNickname(), chanName).c_str(), ERR_NOSUCHCHANNEL(server.getUser(myfd).getNickname(), chanName).size(), 0);
		return false;
	}
	Channel	&chan = getChannel(chanName);
	if (chan.isOperator(myfd) == false) {
		send(myfd, ERR_CHANOPRIVSNEEDED(server.getUser(myfd).getNickname(), chanName).c_str(), ERR_CHANOPRIVSNEEDED(server.getUser(myfd).getNickname(), chanName).size(), 0);
		return false;
	}
	if (params.size() == 1) {
		std::string mode = "";
		if (chan.getIsTopic())
			mode += "t";
		if (chan.isInvOnly())
			mode += "i";
		if (chan.isKeyMode())
			mode += "k";
		if (chan.isLimitMode())
			mode += "l";
		send(myfd, RPL_CHANNELMODEIS(server.getUser(myfd).getNickname(), chanName, mode).c_str(), RPL_CHANNELMODEIS(server.getUser(myfd).getNickname(), chanName, mode).size(), 0);
		return true;
	}
	pos = params[1].find("\r\n");
	if (pos != std::string::npos)
		params[1] = params[1].substr(0, pos);
	if (params[1][0] != '+' && params[1][0] != '-') {
		send(myfd, ERR_UMODEUNKNOWNFLAG(server.getUser(myfd).getNickname()).c_str(), ERR_UMODEUNKNOWNFLAG(server.getUser(myfd).getNickname()).size(), 0);
		return false;
	}
	if (params[1].size() != 2) {
		send(myfd, ERR_UMODEUNKNOWNFLAG(server.getUser(myfd).getNickname()).c_str(), ERR_UMODEUNKNOWNFLAG(server.getUser(myfd).getNickname()).size(), 0);
		return true;
	}
	bool isAdd = false;
	if (params[1][0] == '+')
		isAdd = true;

	if (params[1][1] == 't') {
		chan.switchCanTopic(isAdd ? true : false);
	}
	else if (params[1][1] == 'i') {
		chan.switchInvOnly(isAdd ? true : false, myfd);
	}
	else if (params[1][1] == 'k') {
		if (isAdd)
			chan.switchKeyMode(params[2]);
		else
			chan.switchKeyMode();
	}
	else if (params[1][1] == 'o') {
		pos = params[2].find("\r\n");
		if (pos != std::string::npos)
			params[2] = params[2].substr(0, pos);
		User	&userTarget = server.getUser(server.getTargetUserFd(params[2]));
		if (isAdd) {
			if (chan.isOperator(userTarget.getFd())) {
				std::cout << params[2] << " is already operator" << std::endl;
				return false;
			} else {
				chan.addOperator(userTarget.getFd());
				rplToSend = params[2] + " has been added to operator";
			}
		} else {
			if (!chan.isOperator(userTarget.getFd())) {
				std::cout << params[2] << " is not operator" << std::endl;
				return false;
			} else {
				chan.revokeOperator(myfd, userTarget.getFd());
				rplToSend = params[2] + " has been revoked from operator";
			}
		}
	}
	else if (params[1][1] == 'l') {
		pos = params[2].find("\r\n");
		if (pos != std::string::npos)
			params[2] = params[2].substr(0, pos);
		if (isAdd) {
			std::stringstream ss(params[2]);
			int num;
			ss >> num;
			chan.switchLimitMode(num);
		} else {
			chan.switchLimitMode();
		}
	} else {
		send(myfd, ERR_UMODEUNKNOWNFLAG(server.getUser(myfd).getNickname()).c_str(), ERR_UMODEUNKNOWNFLAG(server.getUser(myfd).getNickname()).size(), 0);
		return false;
	}
	send(myfd, RPL_CHANNELMODEIS(server.getUser(myfd).getNickname(), chanName, params[1]).c_str(), RPL_CHANNELMODEIS(server.getUser(myfd).getNickname(), chanName, params[1]).size(), 0);

	return true;
}
