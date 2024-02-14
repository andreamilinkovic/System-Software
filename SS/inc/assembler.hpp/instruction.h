#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include<stdint.h>
#include<map>
#include<queue>
#include "symbolTable.h"
#include "token.h"

using namespace std;

struct InstructionInfo{
  uint8_t opcode;
  uint16_t numOfOperands;

  InstructionInfo(uint8_t opcode_, uint16_t numOfOperands_): opcode(opcode_), numOfOperands(numOfOperands_){}
};

enum AddressingType{
  IMMEDIATE,
  REGISTER_DIRECT,
  REGISTER_INDIRECT,
  REGISTER_INDIRECT_WITH_SIGNED_OFFSET,
  MEMORY_DIRECT,
  REGISTER_DIRECT_WITH_SIGNED_OFFSET
};

enum UpdateType{
  NO_UPDATE,
  DEC_BY_2_BEFORE_FORMING_ADDRESS,
  INC_BY_2_BEFORE_FORMING_ADDRESS,
  DEC_BY_2_AFTER_FORMING_ADDRESS,
  INC_BY_2_AFTER_FORMING_ADDRESS
};

class Instruction{
  private:
    static map<string, InstructionInfo> allInstructions;
    static map<AddressingType, uint8_t> allAddressingModes;
    static map<UpdateType, uint8_t> allUpdateModes;

  public:
    static uint16_t processInstruction(vector<sym_table_entry>& sym_table, string mnemonic, queue<string> operands, 
      unsigned long lc, string curr_section, unsigned long lineNo);
    
    static void processSymbol(uint16_t lc, relocation_type type, string curr_section_name, string operand);

    static TokenType getOperandTokenType(queue<string> operands);
};

#endif