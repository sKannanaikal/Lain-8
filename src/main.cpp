/**
 * @file main.cpp
 * @author Arbiter (Sean Kannanaikal)
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

<<<<<<< Updated upstream
void _print_header(std::string file_path)
{	
	// TODO update the erro handling and refactor it but for now just get the functiaonlaitity out
	std::ifstream 	file_reader_stream(file_path, std::ios::in);
	if (!file_reader_stream.is_open()) 
	{
		std::println("Error opening file");
		return;
	}
	
	std::string line;
	while (std::getline(file_reader_stream, line))
	{
		std::println("{}", line);
	}
}
=======
>>>>>>> Stashed changes

/**
* @brief entry point for Lain-8 shell interpreter
* 
* @return int status code
*/
int main()
{
<<<<<<< Updated upstream
	//TODO implement barebones shell interface that prints out title and ascii art https://cppreference.com/cpp/numeric/random
	//TODO for now just hardcode the headers into the program but for later use an install script or something else with a fixed path
	std::println("{}", headers[0]);
	std::println("{}", art[0]);
	std::print("> ");
	return 0; //todo figure out error handling mechanism (status codes, std::expected or try catches) https://www.youtube.com/watch?v=Vz40rDiWnN8
=======
	Lain8::execute_lain_shell();
	return 0;
>>>>>>> Stashed changes
}
