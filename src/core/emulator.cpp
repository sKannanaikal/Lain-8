/** @file emulator.cpp
 * @brief CHIP-8 initialization, instruction decoding, and opcode handlers.
 */

#include <core/emulator.hpp>
#include <core/screen.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <print>
#include <random>
#include <SDL3/SDL.h>
#include <thread>

namespace Lain8 {

Emulator::Emulator() {
  std::fill_n(registers, 16, 0);
  std::fill_n(memory, 4096, 0);
  std::fill_n(screen_buffer, 2048, 0);
  delay_timer = 0;
  sound_timer = 0;
  i_register = 0;
  program_counter = 0;
  stack_pointer = 0;

  // populating fonts
  std::uint8_t char0_repr[5] = {0xF0, 0x90, 0x90, 0x90, 0xF0}; // 0
  std::uint8_t char1_repr[5] = {0x20, 0x60, 0x20, 0x20, 0x70}; // 1
  std::uint8_t char2_repr[5] = {0xF0, 0x10, 0xF0, 0x80, 0xF0}; // 2
  std::uint8_t char3_repr[5] = {0xF0, 0x10, 0xF0, 0x10, 0xF0}; // 3
  std::uint8_t char4_repr[5] = {0x90, 0x90, 0xF0, 0x10, 0xF0}; // 4
  std::uint8_t char5_repr[5] = {0xF0, 0x80, 0xF0, 0x10, 0xF0}; // 5
  std::uint8_t char6_repr[5] = {0xF0, 0x80, 0xF0, 0x90, 0xF0}; // 6
  std::uint8_t char7_repr[5] = {0xF0, 0x10, 0x20, 0x40, 0x40}; // 7
  std::uint8_t char8_repr[5] = {0xF0, 0x90, 0xF0, 0x90, 0xF0}; // 8
  std::uint8_t char9_repr[5] = {0xF0, 0x90, 0xF0, 0x10, 0xF0}; // 9
  std::uint8_t charA_repr[5] = {0xF0, 0x90, 0xF0, 0x90, 0x90}; // A
  std::uint8_t charB_repr[5] = {0xE0, 0x90, 0xE0, 0x90, 0xE0}; // B
  std::uint8_t charC_repr[5] = {0xF0, 0x80, 0x80, 0x80, 0xF0}; // C
  std::uint8_t charD_repr[5] = {0xE0, 0x90, 0x90, 0x90, 0xE0}; // D
  std::uint8_t charE_repr[5] = {0xF0, 0x80, 0xF0, 0x80, 0xF0}; // E
  std::uint8_t charF_repr[5] = {0xF0, 0x80, 0xF0, 0x80, 0x80}; // F

  _populate_font(char0_repr, 0);
  _populate_font(char1_repr, 5);
  _populate_font(char2_repr, 10);
  _populate_font(char3_repr, 15);
  _populate_font(char4_repr, 20);
  _populate_font(char5_repr, 25);
  _populate_font(char6_repr, 30);
  _populate_font(char7_repr, 35);
  _populate_font(char8_repr, 40);
  _populate_font(char9_repr, 45);
  _populate_font(charA_repr, 50);
  _populate_font(charB_repr, 55);
  _populate_font(charC_repr, 60);
  _populate_font(charD_repr, 65);
  _populate_font(charE_repr, 70);
  _populate_font(charF_repr, 75);
}

void Emulator::_populate_font(std::uint8_t font_repr[],
                              std::uint8_t memory_index) {
  for (std::uint8_t i = memory_index; i < (memory_index + 5); i++) {
    this->memory[i] = font_repr[i - memory_index];
  }
}

void Emulator::load_program(std::uint8_t program_bytes[]) {
  // get the proper size of a program and pass it in here
  std::copy(&program_bytes[0], &program_bytes[3584], &this->memory[0x200]);
  this->program_counter = 0x200;

  std::jthread cpu_thread(
      [this](std::stop_token stop) { execute_program(stop); });

  mScreen.mainLoop();
}

void Emulator::execute_program(std::stop_token stop) {
  while (!stop.stop_requested() && program_counter + 1 < sizeof(memory)) {
    step();
  }
}

void Emulator::step() {
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
  if (stack_pointer == std::size(stack))
    return;
  stack[stack_pointer++] = program_counter;
  this->program_counter = fun_address;
}

void Emulator::ret_from_func() {
  if (stack_pointer != 0)
    program_counter = stack[--stack_pointer];
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

void Emulator::skip_if_key_pressed(std::uint8_t key_code) {
  if (mScreen.examineKeyStates(key_code) == true)
    this->program_counter += 2;
}

void Emulator::skip_if_not_key_pressed(std::uint8_t key_code) {
  if (mScreen.examineKeyStates(key_code) == false)
    this->program_counter += 2;
}

void Emulator::get_pressed_key(std::uint8_t register_num) {
  if (const auto key_code = mScreen.waitForPressedKey())
    this->registers[register_num] = *key_code;
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
  const int sum = registers[first_reg] + registers[second_reg];
  registers[first_reg] = static_cast<std::uint8_t>(sum);
  registers[0xF] = sum > 0xFF;
}

void Emulator::sub_registers(std::uint8_t first_reg, std::uint8_t second_reg) {
  const int difference = registers[first_reg] - registers[second_reg];
  registers[first_reg] = static_cast<std::uint8_t>(difference);
  registers[0xF] = difference >= 0;
}

void Emulator::sub_registers_reverse(std::uint8_t first_reg,
                                     std::uint8_t second_reg) {
  const int difference = registers[second_reg] - registers[first_reg];
  registers[first_reg] = static_cast<std::uint8_t>(difference);
  registers[0xF] = difference >= 0;
}

// configurable
void Emulator::shift_right(std::uint8_t first_reg, std::uint8_t second_reg) {
  const std::uint8_t value = registers[second_reg];
  registers[first_reg] = value >> 1;
  registers[0xF] = value & 1;
}

// configurable
void Emulator::shift_left(std::uint8_t first_reg, std::uint8_t second_reg) {
  const std::uint8_t value = registers[second_reg];
  registers[first_reg] = static_cast<std::uint8_t>(value << 1);
  registers[0xF] = value >> 7;
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

void Emulator::convert_to_decimal(std::uint8_t register_num) {
  const std::uint8_t value = registers[register_num];
  memory[i_register] = value / 100;
  memory[i_register + 1] = (value / 10) % 10;
  memory[i_register + 2] = value % 10;
}

void Emulator::get_font_address(std::uint8_t register_num) {
  std::uint8_t desired_character = this->registers[register_num];
  this->i_register = desired_character * 5;
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
  constexpr int width = 64;
  constexpr int height = 32;
  const int x = registers[x_register] % width;
  const int y = registers[y_register] % height;
  registers[0xF] = 0;

  for (int row = 0; row < num_pixels && y + row < height &&
                    i_register + row < static_cast<int>(std::size(memory));
       ++row) {
    const std::uint8_t sprite = memory[i_register + row];
    for (int column = 0; column < 8 && x + column < width; ++column) {
      if ((sprite & (0x80 >> column)) == 0)
        continue;

      auto &pixel = screen_buffer[(y + row) * width + x + column];
      if (pixel != 0)
        registers[0xF] = 1;
      pixel ^= 0xFFFFFFFF;
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
    std::uint8_t register_num = (instruction & 0x0F00) >> 8;
    std::uint8_t key_code = this->registers[register_num];
    switch (instruction & 0x00FF) {
    case 0x9E:
      skip_if_key_pressed(key_code);
      break;
    case 0xA1:
      skip_if_not_key_pressed(key_code);
      break;
    }
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
