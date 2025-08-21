#pragma once
#include <iostream>
#include <string>
#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"

class Channel;

class Server;

class User{

	public :

	//Constructor & Destructor
		User(User const &copy);
		User(unsigned int fd);
		~User();

	//Surcharge operator
		User	&operator=(User const &other);

	// Getter
		std::string					getToken();
		int							getTimeToken();
		int							getFd();
		std::string					getNickname();
		std::string					getUsername();
		std::string					getBuffer();
		std::string					getMyChannel();
		std::vector<std::string>	&getMyChannels();
		int 						findChannelIndex(std::string myChannel);
		bool						getRegisterCap();
		bool						getRegisterPass();
		bool						getRegisterNick();
		bool						getRegisterUser();


	//Setter
		void	setFd(unsigned int fd);
		void	setNickname(std::string nickname);
		void	setUsername(std::string username);
		void	setBuffer(std::string buffer);
		void	setMyChannel(std::string myChannel);
		void	setToken(std::string token);
		void	setTimeToken(int toker);
		void	setRegisterCapTrue();
		void	setRegisterCapFalse();
		void	setRegisterPassTrue();
		void	setRegisterPassFalse();
		void	setRegisterNickTrue();
		void	setRegisterNickFalse();
		void	setRegisterUserTrue();
		void	setRegisterUserFalse();

	//Member Function
		void	PRIVMSG(std::vector<std::string> &params, unsigned int clientFd, Server &other);

	private :
		User();
	//Variable member
		std::string					_token;
		int							_timeToken;
		unsigned int				_fd;
		std::string					_nickname;
		std::string					_username;
		std::string					_buffer;
		std::string					_myChannel;
		std::vector<std::string>	_myChannels;
		bool						_registerCap;
		bool						_registerPass;
		bool						_registerNick;
		bool						_registerUser;

};
