#include <iostream>
#include <string>
#include <csignal>
#include "../includes/Server.hpp"
#include "../includes/handleInclude.hpp"

int	main(int argc, char ** argv) {

	parsing(argc, argv);

	Server &server = Server::getInstance();
	server.initServer();
	server.run();

	return 0;
}
