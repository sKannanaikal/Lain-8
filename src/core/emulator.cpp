#include "emulator.hpp" // TODO update this to use <emulator.hpp> instead
#include "screen.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <print>
#include <random>
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

void Emulator::call(std::uint16_t fun_address) {
  this->stack[this->stack_pointer] = this->program_counter;
  this->stack_pointer += 1;
  this->program_counter = fun_address;
}

void Emulator::ret_from_func() {
  this->program_counter = this->stack[this->stack_pointer];
  this->stack_pointer -= 1;
}

void Emulator::skip_neq_reg(std::uint8_t first_register_num,
                            std::uint8_t second_register_num) {
  if (this->registers[first_register_num] !=
      this->registers[second_register_num])
    this->program_counter += 2;
}

void Emulator::skip_eq_reg(std::uint8_t first_register_num,
                           std::uint8_t second_register_num) {
  if (this->registers[first_register_num] ==
      this->registers[second_register_num])
    this->program_counter += 2;
}

void Emulator::skip_neq_val(std::uint8_t register_num, std::uint8_t cmp_value) {
  if (this->registers[register_num] != cmp_value)
    this->program_counter += 2;
}

void Emulator::skip_eq_val(std::uint8_t register_num, std::uint8_t cmp_value) {
  if (this->registers[register_num] == cmp_value)
    this->program_counter += 2;
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

void Emulator::set_from_register_value(std::uint8_t dst, std::uint8_t src) {
  this->registers[dst] = this->registers[src];
}

void Emulator::binary_or(std::uint8_t first_reg, std::uint8_t second_reg) {
  this->registers[first_reg] =
      this->registers[first_reg] | this->registers[second_reg];
}
void Emulator::binary_and(std::uint8_t first_reg, std::uint8_t second_reg) {
  this->registers[first_reg] =
      this->registers[first_reg] & this->registers[second_reg];
}
void Emulator::logical_xor(std::uint8_t first_reg, std::uint8_t second_reg) {
  this->registers[first_reg] =
      this->registers[first_reg] ^ this->registers[second_reg];
}
void Emulator::add_registers(std::uint8_t first_reg, std::uint8_t second_reg) {
  if (0xFF - this->registers[first_reg] <= this->registers[second_reg]) {
    this->registers[0xF] = 1;
  } else {
    this->registers[0xF] = 0;
  }
  this->registers[first_reg] =
      this->registers[first_reg] + this->registers[second_reg];
}
void Emulator::sub_registers(std::uint8_t first_reg, std::uint8_t second_reg) {
  if (this->registers[first_reg] >= this->registers[second_reg]) {
    this->registers[0xF] = 1;
  } else {
    this->registers[0xF] = 0;
  }

  this->registers[first_reg] =
      this->registers[first_reg] - this->registers[second_reg];
}
void Emulator::sub_registers_reverse(std::uint8_t first_reg,
                                     std::uint8_t second_reg) {
  if (this->registers[second_reg] >= this->registers[first_reg]) {
    this->registers[0xF] = 1;
  } else {
    this->registers[0xF] = 0;
  }

  this->registers[first_reg] =
      this->registers[second_reg] - this->registers[first_reg];
}

// configurable
void Emulator::shift_right(std::uint8_t first_reg, std::uint8_t second_reg) {
  this->registers[first_reg] = this->registers[second_reg]; // configurable
  this->registers[first_reg] = this->registers[first_reg] >> 1;
}

// configurable command
void Emulator::shift_left(std::uint8_t first_reg, std::uint8_t second_reg) {
  this->registers[first_reg] = this->registers[second_reg]; // configurable
  this->registers[first_reg] = this->registers[first_reg] << 1;
}

// configurable command
void Emulator::jump_with_offset(std::uint16_t address) {
  this->program_counter = this->registers[0] + address;
}
void Emulator::clear_screen() {
  std::fill_n(this->screen_buffer, 2048, 0x00000000);
  mScreen.render(this->screen_buffer);
}

void Emulator::gen_random_num(std::uint8_t result_reg_num, std::uint8_t value) {
  std::random_device seed;
  std::mt19937 engine(seed());

  std::uniform_int_distribution<> rng(0, 255);
  int random_num = rng(engine);

  this->registers[result_reg_num] = random_num & value;
}

void Emulator::set_register_to_timer_value(std::uint8_t register_num) {
  this->registers[register_num] = this->delay_timer;
}

void Emulator::set_delay_timer(std::uint8_t register_num) {
  this->delay_timer = this->registers[register_num];
}

void Emulator::set_sound_timer(std::uint8_t register_num) {
  this->sound_timer = this->registers[register_num];
}

void Emulator::add_to_index(std::uint8_t register_num) {
  this->i_register += this->registers[register_num];
}

void Emulator::store_registers_in_memory(std::uint8_t max_register_num) {
  for (int i = 0; i <= max_register_num; i++) {
    this->memory[this->i_register + i] = this->registers[i];
  }
}

void Emulator::grab_registers_from_memory(std::uint8_t max_register_num) {
  for (int i = 0; i <= max_register_num; i++) {
    this->registers[i] = this->memory[this->i_register + i];
  }
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
    if (instruction == 0x00E0) {
      clear_screen();
    } else if (instruction == 0x00EE) {
      ret_from_func();
    }
    break;
  }
  case 0x1: {
    std::uint16_t jmp_address = 0x0FFF & instruction;
    jump(jmp_address);
    break;
  }
  case 0x2: {
    std::uint16_t fun_address = 0x0FFF & instruction;
    call(fun_address);
    break;
  }
  case 0x3: {
    std::uint8_t register_num = (instruction & 0x0F00) >> 8;
    std::uint8_t value = (instruction & 0x00FF);
    skip_eq_val(register_num, value);
    break;
  }
  case 0x4: {
    std::uint8_t register_num = (instruction & 0x0F00) >> 8;
    std::uint8_t value = (instruction & 0x00FF);
    skip_neq_val(register_num, value);
    break;
  }
  case 0x5: {
    std::uint8_t first_register_num = (instruction & 0x0F00) >> 8;
    std::uint8_t second_register_num = (instruction & 0x00F0) >> 4;
    skip_eq_reg(first_register_num, second_register_num);
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
    std::uint8_t first_register_num = (instruction & 0x0F00) >> 8;
    std::uint8_t second_register_num = (instruction & 0x00F0) >> 4;
    switch (instruction & 0x000F) {
    case 0:
      set_from_register_value(first_register_num, second_register_num);
      break;
    case 1:
      binary_or(first_register_num, second_register_num);
      break;
    case 2:
      binary_and(first_register_num, second_register_num);
      break;
    case 3:
      logical_xor(first_register_num, second_register_num);
      break;
    case 4:
      add_registers(first_register_num, second_register_num);
      break;
    case 5:
      sub_registers(first_register_num, second_register_num);
      break;
    case 6:
      shift_right(first_register_num, second_register_num);
      break;
    case 7:
      sub_registers_reverse(first_register_num, second_register_num);
      break;
    case 0xE:
      shift_left(first_register_num, second_register_num);
      break;
    }
    break;
  }
  case 0x9: {
    std::uint8_t first_register_num = (instruction & 0x0F00) >> 8;
    std::uint8_t second_register_num = (instruction & 0x00F0) >> 4;
    skip_neq_reg(first_register_num, second_register_num);
    break;
  }
  case 0xA: {
    std::uint16_t register_val = 0x0FFF & instruction;
    set_index_register(register_val);
    // std::exit(-1);
    break;
  }
  case 0xB: {
    std::uint16_t address = 0x0FFF & instruction;
    jump_with_offset(address);
    break;
  }
  case 0xC: {
    std::uint8_t register_num = (0x0F00 & instruction) >> 8;
    std::uint8_t value = (0x00FF & instruction);
    gen_random_num(register_num, value);
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
    std::uint8_t register_num = (instruction & 0x0F00) >> 8;
    std::uint8_t secondary_instr_type = (instruction & 0x00FF);
    switch (secondary_instr_type) {
    case 0x07:
      set_register_to_timer_value(register_num);
      break;
    case 0x15:
      set_delay_timer(register_num);
      break;
    case 0x18:
      set_sound_timer(register_num);
      break;
    case 0x1E:
      add_to_index(register_num);
      break;
    case 0x0A:
      get_pressed_key(register_num);
      break;
    case 0x29:
      get_font_address(register_num);
      break;
    case 0x33:
      convert_to_decimal(register_num);
      break;
    case 0x55:
      store_registers_in_memory(register_num);
      break;
    case 0x65:
      grab_registers_from_memory(register_num);
      break;
    }
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
