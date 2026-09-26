#include <iostream>
#include <print>
#include <string>

#include "shell.hpp"

namespace Lain8
{	
enum class LainCommand
{
	LOAD,
	PAUSE,
	DEBUG,
	QUIT,
	UNKNOWN,
};

static inline std::string _lainCommandToString(LainCommand command)
{
	switch(command)
	{
		case LainCommand::LOAD: 	return "LOAD";
		case LainCommand::PAUSE: 	return "PAUSE";
		case LainCommand::DEBUG: 	return "DEBUG";
		case LainCommand::QUIT: 	return "QUIT";
		default:			return "UNKNOWN";
	}
}

static inline LainCommand _stringToLainCommand(const std::string& input)
{
	if (input == "LOAD")
		return LainCommand::LOAD;
	else if (input == "PAUSE")
		return LainCommand::PAUSE;
	else if (input == "DEBUG")
		return LainCommand::DEBUG;
	else if (input == "QUIT")
		return LainCommand::QUIT;
	return LainCommand::UNKNOWN;
}

static inline void _executeLoad()
{
	std::string filepath = "";
	std::print("Enter a filepath to a CHIP-8 program: ");
	std::getline(std::cin, filepath);

	//TODO verify program exists IFF then load into Chip8 system memory
	return;
}

static inline void _executePause()
{
	return;
}

static inline void _executeDebug()
{
	return;
}

static inline void _executeUnknown()
{
	std::println("Unknown Command");
}

static inline void _executeQuit(bool& should_quit)
{
	should_quit = true;
}
void execute_lain_shell()
{
	std::string input_command = "";
	LainCommand command = LainCommand::UNKNOWN;
	bool should_quit = false;
	while (!should_quit)
	{
		std::print("> ");
		std::getline(std::cin, input_command);	
		command = _stringToLainCommand(input_command);
		switch (command)
		{
			case LainCommand::LOAD: 	_executeLoad(); break;
			case LainCommand::PAUSE: 	_executePause(); break;
			case LainCommand::DEBUG: 	_executeDebug(); break;
			case LainCommand::QUIT: 	_executeQuit(should_quit); break;
			default:			_executeUnknown();
		}
	}
}
}
