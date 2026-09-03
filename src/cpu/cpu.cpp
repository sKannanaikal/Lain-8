#include <cstdint>

#include "cpu.hpp"

// general purpose registers
std::uint8_t v0 = 0;
std::uint8_t v1 = 0;
std::uint8_t v2 = 0;
std::uint8_t v3 = 0;
std::uint8_t v4 = 0;
std::uint8_t v5 = 0;
std::uint8_t v6 = 0;
std::uint8_t v7 = 0;
std::uint8_t v8 = 0;
std::uint8_t v9 = 0;
std::uint8_t vA = 0;
std::uint8_t vB = 0;
std::uint8_t vC = 0;
std::uint8_t vD = 0;
std::uint8_t vE = 0;
std::uint8_t vF = 0;

// register used for memory
std::uint16_t I = 0;

// register for delay and sound timer
std::uint8_t delay_timer = 0;
std::uint8_t sound_timer = 0;