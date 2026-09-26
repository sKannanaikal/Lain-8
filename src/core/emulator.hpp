#include <cstdint>

#include "emulator.hpp"

namespace Lain8
{

class emulator
{

public:
	//TODO define these values actually in the constructor just name the variables here
	std::uint8_t registers[16] = {0};
	std::uint8_t memory[4096] = {0};
	std::uint8_t delay_timer = 0;
	std::uint8_t sound_timer = 0;
	std::uint16_t i_register = 0;
	std::uint16_t program_counter = 0;
	std::uint8_t stack_pointer = 0;
	std::uint16_t stack[16] = {0};

	void execute_program(std::uint8_t program_bytes[]);
};

}
