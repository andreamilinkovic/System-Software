#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <iomanip>

const uint8_t HALT = 0x00;
const uint8_t INT = 0x10;
const uint8_t IRET = 0x20;
const uint8_t CALL = 0x30;
const uint8_t RET = 0x40;
const uint8_t JMP = 0x50;
const uint8_t JEQ = 0x51;
const uint8_t JNE = 0x52;
const uint8_t JGT = 0x53;
const uint8_t XCHG = 0x60;
const uint8_t ADD = 0x70;
const uint8_t SUB = 0x71;
const uint8_t MUL = 0x72;
const uint8_t DIV = 0x73;
const uint8_t CMP = 0x74;
const uint8_t NOT = 0x80;
const uint8_t AND = 0x81;
const uint8_t OR = 0x82;
const uint8_t XOR = 0x83;
const uint8_t TEST = 0x84;
const uint8_t SHL = 0x90;
const uint8_t SHR = 0x91;
const uint8_t LDR = 0xa0;
const uint8_t STR = 0xb0;


const uint8_t IMMEDIATE = 0x00;
const uint8_t REGISTER_DIRECT = 0x01;
const uint8_t REGISTER_INDIRECT = 0x02;
const uint8_t REGISTER_INDIRECT_WITH_SIGNED_OFFSET = 0x03;
const uint8_t MEMORY_DIRECT = 0x04;
const uint8_t REGISTER_DIRECT_WITH_SIGNED_OFFSET = 0x05;

const uint8_t NO_UPDATE = 0x00;
const uint8_t DEC_BY_2_BEFORE_FORMING_ADDRESS = 0x01;
const uint8_t INC_BY_2_BEFORE_FORMING_ADDRESS = 0x02;
const uint8_t DEC_BY_2_AFTER_FORMING_ADDRESS = 0x03;
const uint8_t INC_BY_2_AFTER_FORMING_ADDRESS = 0x04;

#endif