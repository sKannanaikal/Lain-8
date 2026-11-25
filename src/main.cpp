/**
 * @file main.cpp
 * @author Sean Kannanaikal
 * @brief Application entry point for the Lain-8 command shell.
 * @version 1.0
 * @date 2026-08-30
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <core/shell.hpp>

/**
 * @brief Run the interactive shell until it exits.
 *
 * @return Zero after the shell exits.
 */
int main() {
  Lain8::execute_lain_shell();
  return 0;
}
