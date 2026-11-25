/** @file instructions.cpp
 * @brief Opcode tests through the emulator's fetch and decode path.
 */

#include <core/emulator.hpp>

#include <algorithm>
#include <gtest/gtest.h>

namespace Lain8 {
namespace {

/// @brief Owns a fresh machine for each test.
class InstructionTest : public testing::Test {
protected:
  Emulator cpu;
  static constexpr std::uint16_t start = 0x200;

  /**
   * @brief Write an opcode to RAM and execute it through Emulator::step().
   * @param opcode Encoded CHIP-8 instruction.
   * @param address RAM address at which to place the instruction.
   */
  Emulator::StepResult run(std::uint16_t opcode,
                           std::uint16_t address = start) {
    cpu.program_counter = address;
    cpu.memory[address] = opcode >> 8;
    cpu.memory[address + 1] = opcode & 0xFF;
    return cpu.step();
  }
};

TEST_F(InstructionTest, Ignore_legacy_call) {
  run(0x0123);
  EXPECT_EQ(cpu.program_counter, start + 2);
}

TEST_F(InstructionTest, Clear_screen) {
  cpu.i_register = 0x300;
  cpu.memory[0x300] = 0x80;
  run(0xD011);
  ASSERT_TRUE(cpu.framebuffer().front());
  cpu.acknowledge_frame();

  run(0x00E0);
  EXPECT_TRUE(std::ranges::none_of(cpu.framebuffer(),
                                  [](bool pixel) { return pixel; }));
  EXPECT_TRUE(cpu.display_dirty());
}

TEST_F(InstructionTest, Return_from_subroutine) {
  run(0x2300);
  run(0x2400, 0x300);
  run(0x00EE, 0x400);
  EXPECT_EQ(cpu.program_counter, 0x302);
  run(0x00EE, 0x302);
  EXPECT_EQ(cpu.program_counter, start + 2);
  EXPECT_EQ(cpu.stack_pointer, 0);
}

TEST_F(InstructionTest, Jump) {
  run(0x1345);
  EXPECT_EQ(cpu.program_counter, 0x345);
}

TEST_F(InstructionTest, Call_subroutine) {
  run(0x2345);
  EXPECT_EQ(cpu.program_counter, 0x345);
  ASSERT_EQ(cpu.stack_pointer, 1);
  EXPECT_EQ(cpu.stack[0], start + 2);
}

TEST_F(InstructionTest, Skip_if_equal_value) {
  cpu.registers[1] = 42;
  run(0x312A);
  EXPECT_EQ(cpu.program_counter, start + 4);
  run(0x312B);
  EXPECT_EQ(cpu.program_counter, start + 2);
}

TEST_F(InstructionTest, Skip_if_not_equal_value) {
  cpu.registers[1] = 42;
  run(0x412A);
  EXPECT_EQ(cpu.program_counter, start + 2);
  run(0x412B);
  EXPECT_EQ(cpu.program_counter, start + 4);
}

TEST_F(InstructionTest, Skip_if_equal_register) {
  run(0x5120);
  EXPECT_EQ(cpu.program_counter, start + 4);
  cpu.registers[2] = 1;
  run(0x5120);
  EXPECT_EQ(cpu.program_counter, start + 2);
}

TEST_F(InstructionTest, Load_value) {
  run(0x61FE);
  EXPECT_EQ(cpu.registers[1], 0xFE);
  EXPECT_EQ(cpu.program_counter, start + 2);
}

TEST_F(InstructionTest, Add_value) {
  cpu.registers[1] = 0xFE;
  cpu.registers[15] = 0xAA;
  run(0x7105);
  EXPECT_EQ(cpu.registers[1], 3);
  EXPECT_EQ(cpu.registers[15], 0xAA);
}

TEST_F(InstructionTest, Copy_register) {
  cpu.registers[2] = 0xA5;
  run(0x8120);
  EXPECT_EQ(cpu.registers[1], 0xA5);
}

TEST_F(InstructionTest, Bitwise_or) {
  cpu.registers[1] = 0xA5;
  cpu.registers[2] = 0x3C;
  run(0x8121);
  EXPECT_EQ(cpu.registers[1], 0xBD);
}

TEST_F(InstructionTest, Bitwise_and) {
  cpu.registers[1] = 0xA5;
  cpu.registers[2] = 0x3C;
  run(0x8122);
  EXPECT_EQ(cpu.registers[1], 0x24);
}

TEST_F(InstructionTest, Bitwise_xor) {
  cpu.registers[1] = 0xA5;
  cpu.registers[2] = 0x3C;
  run(0x8123);
  EXPECT_EQ(cpu.registers[1], 0x99);
}

TEST_F(InstructionTest, Add_registers) {
  cpu.registers[1] = 254;
  cpu.registers[2] = 1;
  run(0x8124);
  EXPECT_EQ(cpu.registers[1], 255);
  EXPECT_EQ(cpu.registers[15], 0);
  run(0x8124);
  EXPECT_EQ(cpu.registers[1], 0);
  EXPECT_EQ(cpu.registers[15], 1);
  cpu.registers[15] = 255;
  run(0x81F4);
  EXPECT_EQ(cpu.registers[1], 255);
  EXPECT_EQ(cpu.registers[15], 0);
}

TEST_F(InstructionTest, Subtract_registers) {
  cpu.registers[1] = cpu.registers[2] = 9;
  run(0x8125);
  EXPECT_EQ(cpu.registers[1], 0);
  EXPECT_EQ(cpu.registers[15], 1);
  run(0x8125);
  EXPECT_EQ(cpu.registers[1], 247);
  EXPECT_EQ(cpu.registers[15], 0);
  cpu.registers[15] = 10;
  run(0x8F25);
  EXPECT_EQ(cpu.registers[15], 1);
}

TEST_F(InstructionTest, Shift_right) {
  cpu.registers[2] = 0x83;
  run(0x8126);
  EXPECT_EQ(cpu.registers[1], 0x41);
  EXPECT_EQ(cpu.registers[15], 1);
  cpu.registers[2] = 2;
  run(0x8126);
  EXPECT_EQ(cpu.registers[15], 0);
}

TEST_F(InstructionTest, Reverse_subtract_registers) {
  cpu.registers[1] = 9;
  cpu.registers[2] = 5;
  run(0x8127);
  EXPECT_EQ(cpu.registers[1], 252);
  EXPECT_EQ(cpu.registers[15], 0);
  cpu.registers[1] = 5;
  run(0x8127);
  EXPECT_EQ(cpu.registers[1], 0);
  EXPECT_EQ(cpu.registers[15], 1);
  cpu.registers[15] = 6;
  run(0x81F7);
  EXPECT_EQ(cpu.registers[1], 6);
}

TEST_F(InstructionTest, Shift_left) {
  cpu.registers[2] = 0xC1;
  run(0x812E);
  EXPECT_EQ(cpu.registers[1], 0x82);
  EXPECT_EQ(cpu.registers[15], 1);
  cpu.registers[2] = 1;
  run(0x812E);
  EXPECT_EQ(cpu.registers[15], 0);
}

TEST_F(InstructionTest, Skip_if_not_equal_register) {
  run(0x9120);
  EXPECT_EQ(cpu.program_counter, start + 2);
  cpu.registers[2] = 1;
  run(0x9120);
  EXPECT_EQ(cpu.program_counter, start + 4);
}

TEST_F(InstructionTest, Load_index) {
  run(0xAF23);
  EXPECT_EQ(cpu.i_register, 0xF23);
}

TEST_F(InstructionTest, Jump_with_offset) {
  cpu.registers[0] = 0x10;
  cpu.registers[3] = 0xFF;
  run(0xB345);
  EXPECT_EQ(cpu.program_counter, 0x355);
}

TEST_F(InstructionTest, Mask_random_byte) {
  cpu.registers[1] = 0xFF;
  run(0xC100);
  EXPECT_EQ(cpu.registers[1], 0);
  for (int i = 0; i < 16; ++i) {
    run(0xC1A5);
    EXPECT_EQ(cpu.registers[1] & ~0xA5, 0);
  }
}

TEST_F(InstructionTest, Draw_sprite) {
  cpu.i_register = 0x300;
  cpu.memory[0x300] = 0xA0;
  cpu.memory[0x301] = 0x40;
  cpu.registers[1] = 3;
  cpu.registers[2] = 4;
  run(0xD122);
  EXPECT_EQ(cpu.framebuffer()[4 * 64 + 3], true);
  EXPECT_EQ(cpu.framebuffer()[4 * 64 + 4], 0);
  EXPECT_EQ(cpu.framebuffer()[4 * 64 + 5], true);
  EXPECT_EQ(cpu.framebuffer()[5 * 64 + 4], true);
  EXPECT_EQ(cpu.registers[15], 0);
  run(0xD122);
  EXPECT_EQ(cpu.framebuffer()[4 * 64 + 3], 0);
  EXPECT_EQ(cpu.registers[15], 1);
}

TEST_F(InstructionTest, Clip_sprite_at_edges) {
  cpu.i_register = 0x300;
  cpu.memory[0x300] = cpu.memory[0x301] = 0xFF;
  cpu.registers[1] = 127;
  cpu.registers[2] = 62;
  run(0xD122);
  EXPECT_EQ(cpu.framebuffer()[30 * 64 + 63], true);
  EXPECT_EQ(cpu.framebuffer()[31 * 64], 0);
  EXPECT_EQ(cpu.framebuffer()[31 * 64 + 63], true);
  cpu.registers[2] = 63;
  run(0xD122);
  EXPECT_EQ(cpu.framebuffer()[31 * 64 + 63], 0);
  EXPECT_EQ(cpu.framebuffer()[63], 0);
}

TEST_F(InstructionTest, Skip_if_key_pressed) {
  cpu.registers[1] = 0xE;
  run(0xE19E);
  EXPECT_EQ(cpu.program_counter, start + 2);
  Emulator::Keypad keys{};
  keys[0xE] = true;
  cpu.set_keys(keys);
  run(0xE19E);
  EXPECT_EQ(cpu.program_counter, start + 4);
}

TEST_F(InstructionTest, Skip_if_key_not_pressed) {
  cpu.registers[1] = 0xE;
  Emulator::Keypad keys{};
  keys[0xE] = true;
  cpu.set_keys(keys);
  run(0xE1A1);
  EXPECT_EQ(cpu.program_counter, start + 2);
  keys[0xE] = false;
  cpu.set_keys(keys);
  run(0xE1A1);
  EXPECT_EQ(cpu.program_counter, start + 4);
}

TEST_F(InstructionTest, Read_delay_timer) {
  cpu.delay_timer = 42;
  run(0xF107);
  EXPECT_EQ(cpu.registers[1], 42);
}

TEST_F(InstructionTest, Wait_for_key) {
  cpu.registers[1] = 0xFF;
  cpu.delay_timer = 2;
  cpu.sound_timer = 1;
  EXPECT_EQ(run(0xF10A), Emulator::StepResult::WaitingForKey);
  EXPECT_EQ(cpu.program_counter, start);
  EXPECT_EQ(cpu.registers[1], 0xFF);

  cpu.tick_timers(1);
  EXPECT_EQ(cpu.delay_timer, 1);
  EXPECT_EQ(cpu.sound_timer, 0);

  cpu.set_keys({}, 0);
  EXPECT_EQ(cpu.step(), Emulator::StepResult::Executed);
  EXPECT_EQ(cpu.program_counter, start + 2);
  EXPECT_EQ(cpu.registers[1], 0);

  Emulator::Keypad keys{};
  keys[0xF] = true;
  cpu.set_keys(keys);
  EXPECT_EQ(run(0xF10A), Emulator::StepResult::Executed);
  EXPECT_EQ(cpu.registers[1], 0xF);
}

TEST_F(InstructionTest, Set_delay_timer) {
  cpu.registers[1] = 25;
  run(0xF115);
  EXPECT_EQ(cpu.delay_timer, 25);
}

TEST_F(InstructionTest, Set_sound_timer) {
  cpu.registers[1] = 25;
  run(0xF118);
  EXPECT_EQ(cpu.sound_timer, 25);
}

TEST_F(InstructionTest, Add_to_index) {
  cpu.i_register = 0x3FF;
  cpu.registers[1] = 2;
  run(0xF11E);
  EXPECT_EQ(cpu.i_register, 0x401);
}

TEST_F(InstructionTest, Load_font_address) {
  cpu.registers[1] = 0xF;
  run(0xF129);
  ASSERT_EQ(cpu.i_register, 75);
  EXPECT_EQ(cpu.memory[cpu.i_register], 0xF0);
  EXPECT_EQ(cpu.memory[cpu.i_register + 4], 0x80);
}

TEST_F(InstructionTest, Store_decimal_digits) {
  cpu.i_register = 0x300;
  cpu.registers[1] = 255;
  run(0xF133);
  EXPECT_EQ(cpu.memory[0x300], 2);
  EXPECT_EQ(cpu.memory[0x301], 5);
  EXPECT_EQ(cpu.memory[0x302], 5);
  cpu.registers[1] = 7;
  run(0xF133);
  EXPECT_EQ(cpu.memory[0x300], 0);
  EXPECT_EQ(cpu.memory[0x301], 0);
  EXPECT_EQ(cpu.memory[0x302], 7);
}

TEST_F(InstructionTest, Store_registers) {
  cpu.i_register = 0x300;
  cpu.registers[0] = 0x12;
  cpu.registers[1] = 0x34;
  cpu.registers[2] = 0x56;
  run(0xF155);
  EXPECT_EQ(cpu.memory[0x300], 0x12);
  EXPECT_EQ(cpu.memory[0x301], 0x34);
  EXPECT_EQ(cpu.memory[0x302], 0);
  EXPECT_EQ(cpu.i_register, 0x300);
}

TEST_F(InstructionTest, Load_registers) {
  cpu.i_register = 0x300;
  cpu.memory[0x300] = 0x12;
  cpu.memory[0x301] = 0x34;
  cpu.memory[0x302] = 0x56;
  run(0xF165);
  EXPECT_EQ(cpu.registers[0], 0x12);
  EXPECT_EQ(cpu.registers[1], 0x34);
  EXPECT_EQ(cpu.registers[2], 0);
  EXPECT_EQ(cpu.i_register, 0x300);
}

} // namespace
} // namespace Lain8
