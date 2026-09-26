/**
 * @file main.cpp
 * @author Sean Kannanaikal
 * @brief 
 * @version 1.0
 * @date 2026-08-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

//std library imports
#include <fstream>
#include <format>
#include <print>
#include <random>

#include "core/shell.hpp"


/**
* @brief entry point for Lain-8 shell interpreter
* 
* @return int status code
*/
int main()
{
	Lain8::execute_lain_shell();
	return 0;
}
