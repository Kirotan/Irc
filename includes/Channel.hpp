#pragma once
#include <iostream>
#include <string>
#include "Server.hpp"
#include "User.hpp"

class Server;

class Channel{

	public :
	//Constructor & Destructor
		Channel(Channel const &copy);
		Channel(std::string name);
		~Channel();

	//Surcharge operator
		Channel	&operator=(Channel const &other);

	//Getter
		std::string				getTopic();
		std::string				getName();
		std::map<int, User*>&	getUsers();
		std::map<int, User*>&	getOpers();
		std::map<int, User*>& 	getInvited();
		User*		getOper(unsigned int fd);
		User*		getUser(int fd);
		bool		isInvited(int fd);
		bool		isKeyMode();
		std::string	getKey();
		bool		isLimitMode();
		bool		isInvOnly();
		size_t		getLimit();
		bool		getIsTopic();

	//Setter
		void	setTopic(unsigned int fd, std::vector<std::string> topic);
		void	displayTopic(unsigned int clientFd, std::string chanName);

	//Member function
		void	addUser(Server &server, unsigned int fd);
		void	removeUser(int clientFd);
		void	removeOperator(unsigned int userFd);
		void	removeInvited(unsigned int userFd);
		void	addOperator(unsigned int fd);
		void	kick(Server &server, int clientFd, std::string nickname, std::string channel_name);
		void	part(Server &server, int clientFd, std::string channel_name);

		void	revokeOperator(unsigned int clientFd, unsigned int userFd);
		bool	isOperator(unsigned int fd);
		bool	isInvited(unsigned int fd);
		void	switchCanTopic(bool val);
		void	switchInvOnly(bool val, int fd);
		void	switchKeyMode();
		void	switchKeyMode(std::string key);
		void	switchLimitMode();
		void	switchLimitMode(int limit);
		void	broadcastChannel(int senderFd, std::string &message);


	private :
	//Member variable
		Channel();
		std::string				_name;
		std::string				_topic;
		bool					_canTopic;
		bool					_invOnly;
		bool					_keyMode;
		bool					_limitMode;
		size_t					_limit;
		std::string				_key;
		std::map<int, User*> 	_users;
		std::map<int, User*>	_operators;
		std::map<int, User*>	_invited;

};
