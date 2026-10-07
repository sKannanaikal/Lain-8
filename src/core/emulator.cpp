#include <algorithm>
#include <cstdint>
#include <cstring>
#include "emulator.hpp" // TODO update this to use <emulator.hpp> instead
#include "../graphics/screen.hpp"

namespace Lain8
{

Emulator::Emulator()
{
	std::uint8_t registers[16] = {0};
	std::uint8_t memory[4096] = {0};
	std::uint8_t delay_timer = 0;
	std::uint8_t sound_timer = 0;
	std::uint16_t i_register = 0;
	std::uint16_t program_counter = 0;
	std::uint8_t stack_pointer = 0;
	std::uint16_t stack[16] = {0};
}

void Emulator::load_program(std::uint8_t program_bytes [])
{
	//get the proper size of a program and pass it in here
	std::copy(&program_bytes[0], &program_bytes[3584], &this->memory[0x200]);
	this->program_counter = 0x200;
	Graphics::Screen display;
	display.turn_on();
}

void Emulator::execute_program()
{
	//TODO think of a better way to find end of program instead of just 3584
	for (int i = this->program_counter; i < 0xE00; i+=2)
	{
		fetch_instr();
	}
}

void Emulator::fetch_instr()
{
	std::uint16_t instruction = this->memory[this->program_counter];
	this->program_counter += 2;
	decode_and_execute_instr(instruction);
}

void Emulator::decode_and_execute_instr(std::uint16_t instruction)
{
	return;
	//core function going to be a switch statement primarily which will call different functio
}

}
