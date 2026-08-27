/**
* @file main.cpp
* @author Arbiter (Sean Kannanaikal)
* @brief main entry point for Lain-8 emulator
* @version 1.0
* @date 2026-08-27
*
* @copyright Copyright (c) 2026
*
*/

//std library imports
#include <print>

//custom library imports
#include "common/return_codes.hpp"

/**
* @brief entry point for Lain-8 emulator
* 
* @return int status code
*/
int main()
{
	std::println("Hello World");
	return static_cast<int>(ReturnCodes::SUCCESS);
}
