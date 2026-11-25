/** @file shell.cpp
 * @brief ROM loading and console command dispatch.
 */

#include <core/emulator.hpp>
#include <core/session.hpp>
#include <core/shell.hpp>

#include <array>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <string_view>

namespace Lain8 {
/// @brief Commands recognized by the interactive shell.
enum class LainCommand {
  LOAD,
  PAUSE,
  DEBUG,
  QUIT,
  UNKNOWN,
};

/// @brief Return the console spelling of a command.
static inline std::string _lainCommandToString(LainCommand command) {
  switch (command) {
  case LainCommand::LOAD:
    return "LOAD";
  case LainCommand::PAUSE:
    return "PAUSE";
  case LainCommand::DEBUG:
    return "DEBUG";
  case LainCommand::QUIT:
    return "QUIT";
  default:
    return "UNKNOWN";
  }
}

/// @brief Parse an uppercase command or return UNKNOWN.
static inline LainCommand _stringToLainCommand(const std::string &input) {
  if (input == "LOAD")
    return LainCommand::LOAD;
  else if (input == "PAUSE")
    return LainCommand::PAUSE;
  else if (input == "DEBUG")
    return LainCommand::DEBUG;
  else if (input == "QUIT")
    return LainCommand::QUIT;
  return LainCommand::UNKNOWN;
}

/** @brief Print byte offsets, hexadecimal values, and printable characters.
 * @param buffer Bytes to display.
 * @param title Heading printed above the dump.
 * @param buffer_size Number of readable bytes in the buffer.
 */
static inline void _hexdump(std::uint8_t buffer[], std::string_view title,
                            int buffer_size) {
  std::println("======== {} ========", title);
  for (int i = 0; i < buffer_size; i++) {
    if (!(i % 16))
      std::print("{:08X} ", i);

    std::print("{:02X} ", buffer[i]);

    if (i % 16 == 15) {
      for (int j = (i - 15); j <= i; j++) {
        if (buffer[j] >= 0x20 && buffer[j] <= 0x7E)
          std::print("{:c}", buffer[j]);
        else
          std::print(".");
      }
      std::println();
    }
  }
  if (buffer_size % 16 != 0)
    std::println();
}

/// @brief Prompt for a ROM path, read the file, and start an emulator session.
static inline void _executeLoad() {
  std::string filepath;
  std::print("Enter a filepath to a CHIP-8 program: ");
  if (!std::getline(std::cin, filepath))
    return;

  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    std::println("Failed to open {}", filepath);
    return;
  }

  std::array<std::uint8_t, Emulator::max_program_size + 1> program;
  file.read(reinterpret_cast<char *>(program.data()), program.size());
  const auto size = static_cast<std::size_t>(file.gcount());
  if (size > Emulator::max_program_size) {
    std::println("ROM exceeds the {}-byte program memory limit",
                 Emulator::max_program_size);
    return;
  }
  if (file.bad() || (file.fail() && !file.eof()) || size == 0) {
    std::println("Could not read a valid CHIP-8 ROM from {}", filepath);
    return;
  }

  _hexdump(program.data(), "Program Bytes", static_cast<int>(size));
  Session session;
  session.run(std::span{program}.first(size));
}

/// @brief Placeholder for the PAUSE command.
static inline void _executePause() { return; }

/// @brief Placeholder for the DEBUG command.
static inline void _executeDebug() { return; }

/// @brief Report an unrecognized command.
static inline void _executeUnknown() { std::println("Unknown Command"); }

/// @brief Set the flag that ends the command loop.
static inline void _executeQuit(bool &should_quit) { should_quit = true; }

void execute_lain_shell() {
  std::string input_command = "";
  LainCommand command = LainCommand::UNKNOWN;
  bool should_quit = false;
  while (!should_quit) {
    std::print("> ");
    if (!std::getline(std::cin, input_command))
      return;
    command = _stringToLainCommand(input_command);
    switch (command) {
    case LainCommand::LOAD:
      _executeLoad();
      break;
    case LainCommand::PAUSE:
      _executePause();
      break;
    case LainCommand::DEBUG:
      _executeDebug();
      break;
    case LainCommand::QUIT:
      _executeQuit(should_quit);
      break;
    default:
      _executeUnknown();
    }
  }
}
} // namespace Lain8
