/** @file emulator.hpp
 * @brief CHIP-8 machine state and instruction execution.
 */

#ifndef CORE_EMULATOR_HPP
#define CORE_EMULATOR_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace Lain8 {

/// @brief Owns CHIP-8 machine state and executes guest instructions.
class Emulator {

public:
  std::uint8_t registers[16]; ///< The sixteen 8-bit V registers, including the
                              ///< VF flag register.
  std::uint8_t memory[4096];  ///< The 4 KiB address space containing font data
                              ///< and the loaded program.
  std::uint8_t delay_timer;   ///< Value exposed to the guest by the delay timer
                              ///< instructions.
  std::uint8_t
      sound_timer; ///< Value written by the guest sound timer instruction.
  std::uint16_t i_register; ///< Index register used for sprite, font, and
                            ///< memory operations.
  std::uint16_t
      program_counter; ///< Byte address of the next instruction to fetch.
  std::uint8_t
      stack_pointer; ///< Number of pending return addresses on the stack.
  std::uint16_t
      stack[16]; ///< Storage for up to sixteen subroutine return addresses.
  static constexpr int display_width = 64;
  static constexpr int display_height = 32;
  static constexpr std::size_t program_start = 0x200;
  static constexpr std::size_t max_program_size = 4096 - program_start;
  using Framebuffer = std::array<bool, display_width * display_height>;
  using Keypad = std::array<bool, 16>;

  enum class StepResult { Executed, WaitingForKey, EndOfMemory };

  Emulator();

  /// Reset the machine and copy a ROM into guest RAM.
  bool load_program(std::span<const std::uint8_t> program);
  StepResult step();
  void tick_timers(std::size_t ticks);
  void set_keys(const Keypad &keys, std::optional<std::uint8_t> pressed = {});

  const Framebuffer &framebuffer() const { return pixels_; }
  bool display_dirty() const { return display_dirty_; }
  void acknowledge_frame() { display_dirty_ = false; }

private:
  Framebuffer pixels_{};
  Keypad keys_{};
  std::optional<std::uint8_t> pressed_key_;
  bool display_dirty_ = true;

  /// @brief Dispatch an opcode using its instruction and operand fields.
  void decode_and_execute_instr(std::uint16_t instruction);
  /// @brief Set the PC to the absolute address (1nnn).
  void jump(std::uint16_t jmp_address);
  /// @brief Load an immediate byte into Vx (6xkk).
  void set_register_value(std::uint8_t register_num,
                          std::uint8_t register_value);
  /// @brief Add an immediate byte to Vx modulo 256 (7xkk).
  void add_to_register(std::uint8_t register_num, std::uint8_t addition_value);
  /// @brief Load an immediate address into I (Annn).
  void set_index_register(std::uint16_t register_value);
  /// @brief Clear the framebuffer and submit it for presentation (00E0).
  void clear_screen();
  /** @brief XOR sprite rows from RAM at I and set VF when a lit pixel is erased
   * (Dxyn).
   * @param x_register Register holding the horizontal origin.
   * @param y_register Register holding the vertical origin.
   * @param num_pixels Number of sprite rows.
   * @details The origin wraps into the display and rows clip at the edges.
   */
  void draw(std::uint8_t x_register, std::uint8_t y_register,
            std::uint8_t num_pixels);
  /// @brief Push the return address and jump if stack space is available
  /// (2nnn).
  void call(std::uint16_t fun_address);
  /// @brief Pop the return address into the PC when the stack contains an entry
  /// (00EE).
  void ret_from_func();
  /// @brief Skip the next instruction when Vx differs from the immediate byte
  /// (4xkk).
  void skip_neq_val(std::uint8_t register_num, std::uint8_t cmp_value);
  /// @brief Skip the next instruction when Vx matches the immediate byte
  /// (3xkk).
  void skip_eq_val(std::uint8_t register_num, std::uint8_t cmp_value);
  /// @brief Skip the next instruction when Vx and Vy differ (9xy0).
  void skip_neq_reg(std::uint8_t first_register_num,
                    std::uint8_t second_register_num);
  /// @brief Skip the next instruction when Vx and Vy match (5xy0).
  void skip_eq_reg(std::uint8_t first_register_num,
                   std::uint8_t second_register_num);
  /// @brief Copy Vy to Vx (8xy0).
  void set_from_register_value(std::uint8_t dst,
                               std::uint8_t src); // from here onwards
  /// @brief Store the bitwise OR of Vx and Vy in Vx (8xy1).
  void binary_or(std::uint8_t first_reg, std::uint8_t second_reg);
  /// @brief Store the bitwise AND of Vx and Vy in Vx (8xy2).
  void binary_and(std::uint8_t first_reg, std::uint8_t second_reg);
  /// @brief Store the bitwise XOR of Vx and Vy in Vx (8xy3).
  void logical_xor(std::uint8_t first_reg, std::uint8_t second_reg);
  /// @brief Add Vy to Vx modulo 256 and store the carry in VF (8xy4).
  void add_registers(std::uint8_t first_reg, std::uint8_t second_reg);
  /// @brief Store Vx minus Vy in Vx and set VF when no borrow is needed (8xy5).
  void sub_registers(std::uint8_t first_reg, std::uint8_t second_reg);
  /// @brief Store Vy minus Vx in Vx and set VF when no borrow is needed (8xy7).
  void sub_registers_reverse(std::uint8_t first_reg, std::uint8_t second_reg);
  /// @brief Shift Vy right into Vx and copy the original low bit to VF (8xy6).
  void shift_right(std::uint8_t first_reg,
                   std::uint8_t second_reg); // configurable
  /// @brief Shift Vy left into Vx and copy the original high bit to VF (8xyE).
  void shift_left(std::uint8_t first_reg,
                  std::uint8_t second_reg); // configurable
  /// @brief Jump to the immediate address plus V0 (Bnnn).
  void jump_with_offset(std::uint16_t address); // configurable
  /// @brief Mask a random byte with kk and store the result in Vx (Cxkk).
  void gen_random_num(std::uint8_t result_reg_num, std::uint8_t value);
  /// @brief Skip the next instruction when the requested key is down (Ex9E).
  void skip_if_key_pressed(std::uint8_t key_code);
  /// @brief Skip the next instruction when the requested key is up (ExA1).
  void skip_if_not_key_pressed(std::uint8_t key_code);
  /// @brief Copy the delay timer value into Vx (Fx07).
  void set_register_to_timer_value(std::uint8_t register_num);
  /// @brief Copy Vx into the delay timer (Fx15).
  void set_delay_timer(std::uint8_t register_num);
  /// @brief Copy Vx into the sound timer (Fx18).
  void set_sound_timer(std::uint8_t register_num);
  /// @brief Add Vx to I (Fx1E).
  void add_to_index(std::uint8_t register_num);
  /// @brief Read a mapped key if one is available.
  std::optional<std::uint8_t> pressed_key();
  /// @brief Set I to the address of the five-byte glyph indexed by Vx (Fx29).
  void get_font_address(std::uint8_t register_num);
  /// @brief Store the hundreds, tens, and ones digits of Vx starting at I
  /// (Fx33).
  void convert_to_decimal(std::uint8_t register_num);
  /// @brief Write V0 through Vx to consecutive bytes starting at I (Fx55).
  void store_registers_in_memory(std::uint8_t max_register_num);
  /// @brief Read consecutive bytes starting at I into V0 through Vx (Fx65).
  void grab_registers_from_memory(std::uint8_t max_register_num);
  /// @brief Copy one five-byte font glyph into RAM at the given offset.
  void _populate_font(std::uint8_t font_repr[], std::uint8_t memory_index);
};

} // namespace Lain8
#endif /* CORE_EMULATOR_HPP */
