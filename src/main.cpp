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

#include "general/resources.hpp"

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

/**
* @brief entry point for Lain-8 shell interpreter
* 
* @return int status code
*/
int main()
{
	std::random_device seed;
	std::mt19937 random_engine(seed());
	std::uniform_int_distribution<> prng(0, 4);
	std::println("{}", headers[prng(random_engine)]);
	std::println("{}", art[prng(random_engine)]);
	return 0; //todo figure out error handling mechanism (status codes, std::expected or try catches) https://www.youtube.com/watch?v=Vz40rDiWnN8
}
