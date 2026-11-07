#include "emulator.hpp" // TODO update this to use <emulator.hpp> instead
#include "screen.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <print>
#include <thread>

namespace Lain8 {

Emulator::Emulator() {
  std::fill_n(registers, 16, 0);
  std::fill_n(memory, 4096, 0);
  std::fill_n(screen_buffer, 2048, 0xFFFFFFFF);
  delay_timer = 0;
  sound_timer = 0;
  i_register = 0;
  program_counter = 0;
  stack_pointer = 0;
}

void Emulator::load_program(std::uint8_t program_bytes[]) {
  // get the proper size of a program and pass it in here
  std::copy(&program_bytes[0], &program_bytes[3584], &this->memory[0x200]);
  this->program_counter = 0x200;

  std::thread cpu_thread(&Emulator::execute_program, this);

  mScreen.mainLoop();
}

void Emulator::execute_program() {
  // TODO think of a better way to find end of program instead of just 3584
  for (int i = this->program_counter; i < 0xE00; i += 2) {
    std::println("Thread started gonna fetch i={:X} | program_counter = {:X}",
                 i, this->program_counter);
    fetch_instr();
    // std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  // TODO create some sort of quit command to Screen to say program done
}

void Emulator::fetch_instr() {
  std::uint16_t instruction =
      (static_cast<std::uint16_t>(this->memory[this->program_counter]) << 8) |
      this->memory[this->program_counter + 1];
  this->program_counter += 2;
  // std::println("Instruction fetched gonna decode and execute {:#04X} located
  // "
  //             "at pc = {:X}",
  // instruction, this->program_counter);
  decode_and_execute_instr(instruction);
}

void Emulator::jump(std::uint16_t jmp_address) {
  // std::println("We are gonna jump from {:x} to {:x}", program_counter,
  //              jmp_address);
  this->program_counter = jmp_address;
}

void Emulator::set_register_value(std::uint8_t register_num,
                                  std::uint8_t register_value) {

  // std::println(
  //     "We are going to set {:x} to register V{:x} with current value {:x}",
  //     register_value, register_num, this->registers[register_num]);
  this->registers[register_num] = register_value;
}

void Emulator::add_to_register(std::uint8_t register_num,
                               std::uint8_t addition_value) {
  // std::println(
  //     "We are going to add {:x} to register V{:x} with current value {:x}",
  //     addition_value, register_num, this->registers[register_num]);
  this->registers[register_num] += addition_value;
}

void Emulator::set_index_register(std::uint16_t register_value) {
  // std::println("Index register being updated from {:x} to {:x}",
  //              this->i_register, register_value);
  this->i_register = register_value;
}

void Emulator::clear_screen() {
  std::fill_n(this->screen_buffer, 2048, 0x00000000);
  mScreen.render(this->screen_buffer);
}

void Emulator::draw(std::uint8_t x_register, std::uint8_t y_register,
                    std::uint8_t num_pixels) {
  std::println("We are going to update V{:X} with x value, and V{:X} with y "
               "value, and draw {:#02X} pixels",
               x_register, y_register, num_pixels);
  std::uint8_t xRegVal =
      this->registers[x_register] % 64; // accounting for wrap around
  std::uint8_t yRegVal =
      this->registers[y_register] % 32; // accounting for wrap around
  this->registers[0xF] = 0;
  std::println("V{:X} (x) = {:#02X} V{:X} (y) = {:#02X}", x_register, xRegVal,
               y_register, yRegVal);
  for (int j = 0; j < num_pixels; j++) {
    std::uint8_t desired_sprite = this->memory[this->i_register + j];
    std::println("this->i_register = {:#04X}", this->i_register);
    std::uint16_t starting_pixel_index = 64 * (yRegVal + j) + xRegVal;
    std::uint8_t pixels_to_draw =
        (starting_pixel_index + 8 < 2048) ? 8 : 2048 - starting_pixel_index;
    std::println("The number of pixels we need to draw is {}", pixels_to_draw);
    for (std::uint8_t i = 0; i < pixels_to_draw; i++) {
      std::uint8_t current_bit = ((desired_sprite) >> (7 - i)) & 0x01;
      if (current_bit == 1) {
        this->screen_buffer[starting_pixel_index + i] =
            this->screen_buffer[starting_pixel_index + i] == 0 ? 0xFFFFFFFF
                                                               : 0x00000000;
        if (this->screen_buffer[starting_pixel_index + i] == 0x00000000)
          this->registers[0xF] = 1;
      }
    }
  }
  mScreen.render(this->screen_buffer);
}

void Emulator::decode_and_execute_instr(std::uint16_t instruction) {
  // get values from bitmasks
  // clean this code up it's nasty
  std::uint16_t first_nibble = 0xF000;
  std::uint16_t second_nibble = 0x0F00;
  std::uint16_t third_nibble = 0x00F0;
  std::uint16_t fourth_nibble = 0x000F;
  std::uint8_t instruction_type = instruction >> 12;
  // TODO enclose all of these individual cases in curly braces
  std::println("Current Instruction is {:4X} and current nibble is {:X}",
               instruction, instruction_type);
  switch (instruction_type) {
  case 0x0: {
    clear_screen();
    break;
  }
  case 0x1: {
    std::uint16_t jmp_address = 0x0FFF & instruction;
    jump(jmp_address);
    break;
  }
  case 0x2: {
    break;
  }
  case 0x3: {
    break;
  }
  case 0x4: {
    break;
  }
  case 0x5: {
    break;
  }
  case 0x6: {
    std::uint8_t register_num = (instruction & 0x0F00) >> 8;
    std::uint8_t register_value = instruction & 0x00FF;
    set_register_value(register_num, register_value);
    break;
  }
  case 0x7: {
    std::uint8_t register_num_another = (instruction & 0x0F00) >> 8;
    std::uint8_t register_value_another = instruction & 0xFF;
    add_to_register(register_num_another, register_value_another);
    break;
  }
  case 0x8: {
    break;
  }
  case 0x9: {
    break;
  }
  case 0xA: {
    std::uint16_t register_val = 0x0FFF & instruction;
    set_index_register(register_val);
    // std::exit(-1);
    break;
  }
  case 0xB: {
    break;
  }
  case 0xC: {
    break;
  }
  case 0xD: {
    std::uint8_t x_register = (instruction & 0x0F00) >> 8;
    std::uint8_t y_register = (instruction & 0x00F0) >> 4;
    std::uint8_t num_pixels = (instruction & 0x000F);
    draw(x_register, y_register, num_pixels);
    break;
  }
  case 0xE: {
    break;
  }
  case 0xF: {
    break;
  }
  default: {
    std::println("[-] Invalid Byte");
    break;
  }
  }
  return;
}

} // namespace Lain8
