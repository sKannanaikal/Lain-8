#ifndef CPU_HPP
#define CPU_HPP

// general purpose registers
extern std::uint8_t v0;
extern std::uint8_t v1;
extern std::uint8_t v2;
extern std::uint8_t v3;
extern std::uint8_t v4;
extern std::uint8_t v5;
extern std::uint8_t v6;
extern std::uint8_t v7;
extern std::uint8_t v8;
extern std::uint8_t v9;
extern std::uint8_t vA;
extern std::uint8_t vB;
extern std::uint8_t vC;
extern std::uint8_t vD;
extern std::uint8_t vE;
extern std::uint8_t vF;

// register used for memory
extern std::uint16_t I;

// register for delay and sound timer
extern std::uint8_t delay_timer;
extern std::uint8_t sound_timer;

// special pseudo registers
extern std::uint16_t program_counter;
extern std::uint16_t stack_pointer;

#endif /** CPU_HPP */