#ifndef CORE_EMULATOR_HPP
#define CORE_EMULATOR_HPP
#include "screen.hpp"
#include <cstdint>

namespace Lain8 {

class Emulator {

public:
  std::uint8_t registers[16];
  std::uint8_t memory[4096];
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
  void call(std::uint16_t fun_address);
  void ret_from_func();
  void skip_neq_val(std::uint8_t register_num, std::uint8_t cmp_value);
  void skip_eq_val(std::uint8_t register_num, std::uint8_t cmp_value);
  void skip_neq_reg(std::uint8_t first_register_num,
                    std::uint8_t second_register_num);
  void skip_eq_reg(std::uint8_t first_register_num,
                   std::uint8_t second_register_num);
  void set_from_register_value(std::uint8_t dst,
                               std::uint8_t src); // from here onwards
  void binary_or(std::uint8_t first_reg, std::uint8_t second_reg);
  void binary_and(std::uint8_t first_reg, std::uint8_t second_reg);
  void logical_xor(std::uint8_t first_reg, std::uint8_t second_reg);
  void add_registers(std::uint8_t first_reg, std::uint8_t second_reg);
  void sub_registers(std::uint8_t first_reg, std::uint8_t second_reg);
  void sub_registers_reverse(std::uint8_t first_reg, std::uint8_t second_reg);
  void shift_right(std::uint8_t first_reg,
                   std::uint8_t second_reg); // configurable
  void shift_left(std::uint8_t first_reg,
                  std::uint8_t second_reg);     // configurable
  void jump_with_offset(std::uint16_t address); // configurable
  void gen_random_num(std::uint8_t result_reg_num, std::uint8_t value);
  void skip_if_key_pressed(std::uint8_t key_code);
  void skip_if_not_key_pressed(std::uint8_t keycode);
  void set_register_to_timer_value(std::uint8_t register_num);
  void set_delay_timer(std::uint8_t register_num);
  void set_sound_timer(std::uint8_t register_num);
  void add_to_index(std::uint8_t register_num);
  void get_pressed_key(std::uint8_t register_num);
  void get_font_address(std::uint8_t register_num);
  void convert_to_decimal(std::uint8_t register_num);
  void store_registers_in_memory(std::uint8_t max_register_num);
  void grab_registers_from_memory(std::uint8_t max_register_num);
};

} // namespace Lain8
#endif /* CORE_EMULATOR_HPP */
