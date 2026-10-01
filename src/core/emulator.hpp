#ifndef CORE_EMULATOR_HPP
#define CORE_EMULATOR_HPP
#include <cstdint>


namespace Lain8
{

class Emulator
{

public:
	std::uint8_t registers[16];
	std::uint8_t memory[4096];
	std::uint8_t delay_timer;
	std::uint8_t sound_timer;
	std::uint16_t i_register;
	std::uint16_t program_counter;
	std::uint8_t stack_pointer;
	std::uint16_t stack[16];

	void load_program(std::uint8_t program_bytes[]);
	void execute_program();

private:
	void fetch_instr();
	void decode_and_execute_instr();
};

}
#endif /* CORE_EMULATOR_HPP */
