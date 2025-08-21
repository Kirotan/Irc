#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include "../includes/IrcMessage.hpp"

IrcMessage	parseIrcMessage(std::string &input){
	std::istringstream	iss;
	IrcMessage			mess;
	std::string			tmp;
	std::string			stash;

	std::string			garbage;
	int					countWord = 0;


	mess.crlf = "\r\n";
	mess.isCommand = true;

	iss.str(input);
	while(iss >> garbage) {
		countWord++;
	}
	if(countWord < 1){
		mess.params.push_back("");
		mess.isCommand = false;
		return mess;
	}
	iss.clear();

	if (input[0] == '/') {
		input.erase(0, 1);
	}
	iss.str(input);
	for (int i = 0; iss >> tmp; i++) {
		if (i == 0) {
			mess.command = tmp;
		} else if (tmp[0] == ':') {
			stash = tmp;
			while (iss >> tmp) {
				stash += " ";
				stash += tmp;
			}
			stash += mess.crlf;
			mess.params.push_back(stash);
		} else {
			mess.params.push_back(tmp);
		}
	}
	if (mess.params.size() == 0) {
		mess.params.push_back("");
	}
	tmp = mess.params[mess.params.size() - 1] + mess.crlf;
	mess.params[mess.params.size() - 1] = tmp;
	return mess;
}
