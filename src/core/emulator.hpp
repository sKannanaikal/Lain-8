#ifndef CORE_EMULATOR_HPP
#define CORE_EMULATOR_HPP
#include "screen.hpp"
#include <cstdint>

namespace Lain8 {

class Emulator {

public:
  std::uint8_t registers[16];
  std::uint16_t memory[4096];
  std::uint8_t delay_timer;
  std::uint8_t sound_timer;
  std::uint16_t i_register;
  std::uint16_t program_counter;
  std::uint8_t stack_pointer;
  std::uint16_t stack[16];
  std::uint32_t screen_buffer[2048];
  Screen mScreen;

  Emulator();
  void load_program(std::uint8_t program_bytes[]);
  void execute_program();

private:
  void fetch_instr();
  void decode_and_execute_instr(std::uint16_t instruction);
  void jump(std::uint16_t jmp_address);
  void set_register_value(std::uint8_t register_num,
                          std::uint8_t register_value);
  void add_to_register(std::uint8_t register_num, std::uint8_t addition_value);
  void set_index_register(std::uint16_t register_value);
  void clear_screen();
  void draw(std::uint8_t x_register, std::uint8_t y_register,
            std::uint8_t num_pixels);
};

} // namespace Lain8
#endif /* CORE_EMULATOR_HPP */
