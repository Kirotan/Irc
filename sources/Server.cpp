#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <algorithm>
#include <termios.h>
#include <csignal>
#include <netdb.h>
#include <stdlib.h>
#include "../includes/IrcMessage.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Tester.hpp"
#include "../includes/handleInclude.hpp"
#include "../includes/rpl.hpp"


//Constructor
	Server::Server() : _servInfo(NULL), _port(0){
		this->_invitationOnly = false;
		this->setNeedPasswTrue();
		this->_version = "1.0";
		this->_nameServer = "pika_server";
		memset(&_hints, 0, sizeof(_hints));

	}
	Server::Server(Server const &copy){(void)copy;}

//Destructor
	Server::~Server(){}

//Surcharge operator
	Server	&Server::operator=(Server const &other){(void)other; return (*this);}

//Getter & Setter
	void			Server::setPort(unsigned short port){this->_port = port;}
	unsigned short	Server::getPort(){return this->_port;}
	void			Server::setPassword(std::string password){this->_passw = password;}
	std::string		Server::getPassw(){return this->_passw;};
	void			Server::setNeedPasswFalse(){this->_needPassw = false;}
	void			Server::setNeedPasswTrue(){this->_needPassw = true;}
	bool			Server::getNeedPassw(){return this->_needPassw;}
	void			Server::setNameServer(std::string name){this->_nameServer = name;}
	void			Server::setVersion(std::string version){this->_version = version;}
	std::string		Server::getNameServer(){return this->_nameServer;}
	std::string		Server::getVersion(){return this->_version;}


	std::vector<Channel*>& Server::getChannels(){
		return _arrayChannel;
	}

Channel	&Server::getChannel(const std::string channelName){
	std::vector<Channel*>::iterator it = _arrayChannel.begin();
	for (; it != _arrayChannel.end(); ++it) {
		if (channelName == (*it)->getName()) {
			return **it;
		}
	}
	return **it;
}

User	&Server::getUser(int fd) {
	for (std::map<int, User*>::iterator it = this->_arrayUser.begin(); it != _arrayUser.end(); it++) {
		if (it->first == fd)
			return *it->second;
	}
	return *_arrayUser.end()->second;
}

std::map<int, User*>& Server::getUsers() {return _arrayUser;}

std::string		Server::getUsername() {
	struct passwd *un;
	uid_t uid = getuid();
	un = getpwuid(uid);

	if (un)
		return std::string(un->pw_name);
	return NULL;
}

unsigned int	Server::getTargetUserFd(std::string nickname) {
	for (std::map<int, User*>::iterator user_it = _arrayUser.begin(); user_it != _arrayUser.end(); ++user_it) {
		if (user_it->second->getNickname() == nickname) {
			return user_it->first;
		}
	}
	std::cerr << "Nickname not found" << std::endl;
	return 0;
}

bool	Server::isUser(int fd) {
	std::map<int, User*>::iterator it = _arrayUser.begin();
	for (; it != _arrayUser.end(); it++) {
		if (it->first == fd)
			return true;
	}
	return false;
}

unsigned short		Server::getBackLogSize(){
	return this->_backLogSize;
}

bool	Server::isChannel(const std::string &channelName) {
	std::vector<Channel*>::iterator it = _arrayChannel.begin();
	for (; it != _arrayChannel.end(); ++it) {
		if (channelName == (*it)->getName())
			return true;
	}
	return false;
}


int Server::fillServinfo()
{
	char	port[6];

	std::sprintf(port, "%d", _port);

	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;


	if(getaddrinfo(NULL, port, &_hints, &_servInfo) != 0)
		return (1);
	return (0);
}


//Member functions
void	Server::initServer(){

//Call of server instance
	Server	&server = Server::getInstance();

	if(fillServinfo() == 1){
		std::cerr << "ERROR : addrinfo doesn't work." << std::endl;
		return;
	}

//Socket creation : for creating communication point, like a FD

	server._serverSocket = socket(_servInfo->ai_family, _servInfo->ai_socktype, _servInfo->ai_protocol);
	if(server._serverSocket == -1){
		close(server._serverSocket);
		std::cerr << "ERROR SOCKET : Socket can't be created." << std::endl;
		exit(1);
	}

//Allow many sockets on same port
	int opt = 1;
	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "ERROR : setsockopt (SO_REUSEADDR) failed." << std::endl;
		exit(1);
	}

//Bind : bound socket with port
	if(bind(server._serverSocket, _servInfo->ai_addr, _servInfo->ai_addrlen) < 0){
		close(server._serverSocket);
		std::cerr << "ERROR BIND : can't create bound between socket and port." << std::endl;
		exit(1);
	}

//Listen :server able to connect in
	server._backLogSize = 128; //queue size
	if(listen(server._serverSocket, server._backLogSize) < 0){
		close(server._serverSocket);
		std::cerr << "ERROR LISTEN : Unable to listen on the socket." << std::endl;
		exit (1);
	}

	freeaddrinfo(_servInfo);
}


void	Server::initEpoll(){

	Server	&server = getInstance();

	server._epollFd = epoll_create1(0);
	if (_epollFd == -1) {
		std::cerr << "ERROR EPOLL: epoll_create1 failed." << std::endl;
		close(server._serverSocket);
		exit(1);
	}

	server._event.data.fd = server._serverSocket;
	server._event.events = EPOLLIN | EPOLLOUT;

	if (epoll_ctl(server._epollFd, EPOLL_CTL_ADD, server._serverSocket, &server._event) == -1){
		std::cerr << "ERROR EPOLL : epoll_ctl_add failed." << std::endl;
		close(server._serverSocket);
		close(server._epollFd);
		exit(1);
	}
}

void	Server::createChannel(Server &server, unsigned int fd, std::string channel_name){
	if (isChannel(channel_name) == true) {
		std::cerr << "ERROR: Channel already exists" << std::endl;
		return;
	}
	Channel *newChannel = new Channel(channel_name);

	(*newChannel).addUser(server, fd);
	(*newChannel).addOperator(fd);
	this->_arrayChannel.push_back(newChannel);
}

//User
void	Server::createUser(int fd, User &user){
	if (_arrayUser.find(fd) != _arrayUser.end()) {
	std::cerr << "ERROR FD " << fd << ": already exists in _arrayUser." << std::endl;
	return;
	}
	this->_arrayUser.insert(std::make_pair(fd, &user));
}

void	Server::deleteUser(int fd){

	if (_arrayUser.find(fd) == _arrayUser.end()) {
		std::cerr << "WARNING : Attempted to delete non-existent user: " << fd << std::endl;
		return;
	}
	for(std::vector<Channel*>::iterator it = this->_arrayChannel.begin(); !_arrayChannel.empty() && it != _arrayChannel.end(); it++){
		if((*it)->getUser(fd) != NULL){
			(*it)->removeUser(fd);
			if((*it)->isOperator(fd))
				(*it)->removeOperator(fd);
			if ((*it)->getUsers().empty()) {
				_arrayChannel.erase(it);
			}
		}
	}
	std::cout << "Client " << fd << " disconnected." << std::endl;

	User *user = _arrayUser[fd];
	this->_arrayUser.erase(fd);
	delete user;
}

void	Server::broadcastAll(int senderFd, std::string &message){
	for(std::map<int, User*>::iterator it = this->_arrayUser.begin(); it != _arrayUser.end(); it++){
		int clientFd = it->first;
		if(clientFd != senderFd){
			ssize_t bytesSent = send(clientFd, message.c_str(), message.size(), 0);
			if (bytesSent == -1) {
				std::cerr << "ERROR BROADCAST : Failed to send message to client " << clientFd << std::endl;
				continue;
			}
		}
	}
}


void	Server::run(){

	Server	&server = Server::getInstance();
	server.initEpoll();

	struct epoll_event	events[this->_backLogSize];
	int					eventCount, clientFd;

	server._addrlen = sizeof(server._serverAddres);

	while(true){
		eventCount = epoll_wait(server._epollFd, events, this->_backLogSize, -1);
		if(eventCount == -1)
			std::cerr << "ERROR EPOLL_WAIT : epoll_wait doesn't work." << std::endl;

		if (events[0].data.fd == server._serverSocket){ //if connection is about main socket, a new client connection is pending
		//accept client connection
			clientFd = accept(server._serverSocket, (struct sockaddr *)&server._serverAddres, &server._addrlen);
			if(clientFd == -1)
				std::cerr << "ERROR ACCEPT : can't connect to socket." << std::endl;


		//add client to epoll
			struct epoll_event	clientEvent;
			clientEvent.data.fd = clientFd;
			clientEvent.events = EPOLLIN;
			if(epoll_ctl(server._epollFd, EPOLL_CTL_ADD, clientFd, &clientEvent) == -1){
				std::cerr << "ERROR EPOLL : epoll_ctl_add failed." << std::endl;
				continue;
			}

		//add client to client array
			User* newUser = new User(clientFd);
			createUser(clientFd, *newUser);

		} else {
		//handle client message
			clientFd = events[0].data.fd;
			char	buffer[512];

			memset(buffer, 0, sizeof(buffer));
			ssize_t	bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
			std::string	mss = buffer;
			if (bytesRead < 0){
				std::cerr << "ERROR RECV : message can't be receive." << std::endl;
				close(clientFd);
				epoll_ctl(server._epollFd, EPOLL_CTL_DEL, clientFd, NULL);
				deleteUser(clientFd);
			} else if (bytesRead == 0){
				close(clientFd);
				epoll_ctl(server._epollFd, EPOLL_CTL_DEL, clientFd, NULL);
				deleteUser(clientFd);
			} else if (mss.empty())
				std::cout << "control D" << std::endl;
			else if (mss[mss.size() - 1] != '\n'){
				server.getUser(clientFd).setBuffer(mss);
				send(clientFd, "^D", 2, 0);
			} else {
				std::string	input = server.getUser(clientFd).getBuffer() + mss;
				server._arrayParams = parseIrcMessage(input);

			//Identification
				if(server.getUser(clientFd).getRegisterUser() == false)
					identification(clientFd, input);

				else if (server._arrayParams.isCommand == false) {
					std::cout << server._arrayUser[clientFd]->getNickname() << ": " << input << std::flush;
				} else if (server._arrayParams.command == "JOIN")
					join(clientFd);
				else if(server._arrayParams.command == "PING")
					handlePing(clientFd);
				else if (server._arrayParams.command == "PART") {
					if (server._arrayParams.params[0][0] == '#')
						server._arrayParams.params[0].erase(0, 1);
					size_t pos = server._arrayParams.params[0].find("\r\n");
					if (pos != std::string::npos)
						server._arrayParams.params[0] = server._arrayParams.params[0].substr(0, pos);
					if (isChannel(server._arrayParams.params[0])) {
						getChannel(server._arrayParams.params[0]).part(server, clientFd, _arrayParams.params[0]);
					} else {
						send(clientFd, ERR_NOSUCHNICK(server.getUser(clientFd).getNickname(), server._arrayParams.params[0]).c_str(), ERR_NOSUCHNICK(server.getUser(clientFd).getNickname(), server._arrayParams.params[0]).size(), 0);
						std::cerr << ": No such channel" << std::endl;
					}
				} else if (server._arrayParams.command == "KICK") {
					if (server._arrayParams.params[0][0] == '#')
						server._arrayParams.params[0].erase(0, 1);
					size_t pos = server._arrayParams.params[0].find("\r\n");
					if (pos != std::string::npos)
						server._arrayParams.params[0] = server._arrayParams.params[0].substr(0, pos);
					if (isChannel(server._arrayParams.params[0]))
						getChannel(server._arrayParams.params[0]).kick(server, clientFd, server._arrayParams.params[1], server._arrayParams.params[0]);
					else {
						send(clientFd, ERR_NOSUCHNICK(server.getUser(clientFd).getNickname(), server._arrayParams.params[0]).c_str(), ERR_NOSUCHNICK(server.getUser(clientFd).getNickname(), server._arrayParams.params[0]).size(), 0);
						std::cerr << ": No such channel" << std::endl;
					}
				} else if (server._arrayParams.command == "INVITE")
					invite(server._arrayParams.params[0], server._arrayParams.params[1], clientFd);
				else if (server._arrayParams.command == "TOPIC") {
					parseTopic(server, clientFd);
				} else if (server._arrayParams.command == "MODE")
					modeCmdParsing(server._arrayParams.params, clientFd);
				else if (server._arrayParams.command == "WHO")
					whoParsing(server._arrayParams.params, clientFd);
				else if (server._arrayParams.command == "PRIVMSG") {
					getUser(clientFd).PRIVMSG(server._arrayParams.params, clientFd, server);
				}
				else if(server._arrayParams.command == "QUIT"){
					quit(clientFd);
				}
				else if(server._arrayParams.command[0] == '/'){
					std::cout << server._arrayParams.command << " is not a valid command." << std::endl;
					continue;
				} else {
					std::cout << server._arrayUser[clientFd]->getNickname() << ": " << input << std::flush;
				}
				server.getUser(clientFd).setBuffer("");
			}
		}
	}
}
