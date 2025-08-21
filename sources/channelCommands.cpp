#include "../includes/Channel.hpp"
#include "../includes/rpl.hpp"

	void 	Channel::kick(Server &server, int clientFd, std::string nickname, std::string channel_name) {
		size_t pos = nickname.find("\r\n");
		if (pos != std::string::npos)
			nickname = nickname.substr(0, pos);
		User* user = getUser(clientFd);
		User* userTarget = getUser(server.getTargetUserFd(nickname));
		int userTargetFd = server.getTargetUserFd(nickname);
		if (!isOperator(clientFd))
			send(clientFd, ERR_CHANOPRIVSNEEDED(server.getUser(clientFd).getNickname(), channel_name).c_str(), ERR_CHANOPRIVSNEEDED(server.getUser(clientFd).getNickname(), channel_name).size(), 0);
		else {
			if (userTarget == NULL) {
				send(clientFd, ERR_USERNOTINCHANNEL(server.getUser(clientFd).getNickname(), nickname, channel_name).c_str(), ERR_USERNOTINCHANNEL(server.getUser(clientFd).getNickname(), nickname, channel_name).size(), 0);
				std::cerr << "User " << nickname << " not found" << std::endl;
				return;
			}
			send(clientFd, RPL_KICK(server.getUser(clientFd).getNickname(), channel_name, nickname).c_str(), RPL_KICK(server.getUser(clientFd).getNickname(), channel_name, nickname).size(), 0);
			std::string message = "KICK " + _name + " " + userTarget->getNickname() + " :You have been kicked by " + user->getNickname() + "\r\n";
			ssize_t bytesSent = send(userTarget->getFd(), message.c_str(), message.size(), 0);
			if (bytesSent == -1) {
				std::cerr << "ERROR KICK : Failed to send message to client " << userTarget->getFd() << std::endl;
				return;
			}
			std::cout << user->getUsername() << " has kicked " << userTarget->getUsername() << std::endl;
			removeUser(userTargetFd);
			removeInvited(userTargetFd);
			revokeOperator(clientFd, userTargetFd);
			userTarget->setMyChannel("");
		}
	}

	void 	Channel::part(Server &server, int clientFd, std::string channel_name) {
		User* user = getUser(clientFd);

		if (server.getChannel(channel_name).getUser(clientFd) == NULL) {
			send(clientFd, ERR_NOTONCHANNEL(server.getUser(clientFd).getNickname(), channel_name).c_str(), ERR_NOTONCHANNEL(server.getUser(clientFd).getNickname(), channel_name).size(), 0);
			std::cerr << ":You're not on that channel" << std::endl;
			return;
		}
		user->getMyChannels().erase(user->getMyChannels().begin() + user->findChannelIndex(user->getMyChannel()));
		removeUser(clientFd);
		removeInvited(clientFd);
		removeOperator(clientFd);
		std::vector<Channel*>::iterator it = server.getChannels().begin();
		for (; it != server.getChannels().end(); ++it) {
			if ((*it)->getName() == channel_name)
				break;
		}
		if (_users.empty())
			server.getChannels().erase(it);
		user->setMyChannel("");
		send(clientFd, RPL_PART(server.getUser(clientFd).getNickname(), server.getUser(clientFd).getUsername(), channel_name).c_str(), RPL_PART(server.getUser(clientFd).getNickname(), server.getUser(clientFd).getUsername(), channel_name).size(), 0);
	}

	void	Channel::setTopic(unsigned int fd, std::vector<std::string> topic) {
		std::string new_string;
		std::vector<std::string>::iterator it = topic.begin();
		std::advance(it, 1);

		for (; it != topic.end(); ++it) {
			new_string += *it + " ";
		}
		if (new_string.size() > 1)
			new_string.erase(new_string.size() - 2, 1);
		User* user = _users.find(fd)->second;
		if (user) {
			if (_canTopic == true) {
				_topic = new_string;
				for (std::map<int, User*>::iterator itone = _users.begin(); itone != _users.end(); ++itone) {
					send(itone->first, RPL_TOPIC(user->getNickname(), _name, new_string).c_str(), RPL_TOPIC(user->getNickname(), _name, new_string).size(), 0);
				}
			}
			else {
				if (isOperator(fd)) {
					_topic = new_string;
					for (std::map<int, User*>::iterator ittwo = _users.begin(); ittwo != _users.end(); ++ittwo) {
						send(ittwo->first, RPL_TOPIC(user->getNickname(), _name, new_string).c_str(), RPL_TOPIC(user->getNickname(), _name, new_string).size(), 0);
					}
				}
				else
					send(fd, ERR_CHANOPRIVSNEEDED(user->getNickname(), _name).c_str(), ERR_CHANOPRIVSNEEDED(user->getNickname(), _name).size(), 0);
			}
		}
	}

	void	Channel::displayTopic(unsigned int clientFd, std::string chanName) {
		User* user = getUser(clientFd);
		if (user) {
			if (_topic.empty())
				send(clientFd, RPL_NOTOPIC(user->getNickname(), chanName).c_str(), RPL_NOTOPIC(user->getNickname(), chanName).size(), 0);
			else
				send(clientFd, RPL_TOPIC(user->getNickname(), chanName, _topic).c_str(), RPL_TOPIC(user->getNickname(), chanName, _topic).size(), 0);
		}
	}
