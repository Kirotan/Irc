#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

struct IrcMessage
{
	std::string					command;
	std::vector<std::string>	params;
	std::string					crlf;
	bool						isCommand;

};

IrcMessage	parseIrcMessage(std::string &input);
void		parseArgs(IrcMessage &mess, std::string &input);
