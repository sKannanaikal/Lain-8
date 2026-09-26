
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <string_view>

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

static inline void _hexdump(std::uint8_t buffer[], std::string_view title, int buffer_size)
{
	std::println("======== {} ========", title);
	for (int i = 0; i < buffer_size; i++)
	{
		if (!(i % 16))
			std::print("{:08X} ", i);

      		std::print("{:02X} ", buffer[i]);

		if (i % 16 == 15)
		{
			for (int j = (i - 15); j < i; j++)
			{
				if (buffer[j] >= 0x20 && buffer[j] <= 0x7E)
					std::print("{:c}", buffer[j]);
				else
					std::print(".");
			}
      			std::println();
		}
	}
}

static inline void _executeLoad()
{
	std::string filepath = "";
	std::print("Enter a filepath to a CHIP-8 program: ");
	std::getline(std::cin, filepath);

	//TODO verify program exists IFF then load into Chip8 system memory
	std::ifstream fileReaderStream(filepath, std::ios::binary);
	if (!fileReaderStream.is_open())
		std::println("Failed to open {}", filepath);
	else
	{
		//this is the max allowed size for a program in CHIP-8
		std::uint8_t program_bytes[3584] = {0};
		fileReaderStream.read(reinterpret_cast<char *>(program_bytes), 3584);
		_hexdump(program_bytes, "Program Bytes", 3584);
		//TODO create an emulator object and then call execute program
	}
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
