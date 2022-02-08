#pragma once
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>

using Command = std::string;
class CommandBlock
{
	std::list<Command> cmdBlock;
	time_t time;

public:
	CommandBlock() = default;
	void add(const Command& cmd)
	{
		if (cmdBlock.empty())
			time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		cmdBlock.push_back(cmd);
	}

	size_t size()
	{
		return cmdBlock.size();
	}

	std::string toString()
	{
		if (cmdBlock.empty())
			return std::string();
		std::stringstream ss;
		ss << "bulk:";
		auto lastElement = --cmdBlock.end();
		auto it = cmdBlock.begin();
		for (it; it != lastElement; it++)
		{
			ss << *it << ", ";
		}
		ss << *it << std::endl;
		return ss.str();
	}

	void displayPrint()
	{
		std::cout << toString();
	}

	void filePrint()
	{
		std::string fileName = "bulk" + std::to_string(time) + ".log";
		std::ofstream file(fileName);
		file <<  toString() << std::endl;
	}

};