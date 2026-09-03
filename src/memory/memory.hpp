#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>

#define MAX_MEMORY_SIZE 4096 //TODO maybe change to const
#define MAX_STACK_SIZE 16

extern std::uint8_t     memory[MAX_MEMORY_SIZE];
extern std::uint16_t    stack[MAX_STACK_SIZE];

#endif /** MEMORY_HPP */