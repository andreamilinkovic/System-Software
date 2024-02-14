#include "instruction.h"
#include "myexception.h"
#include "sectionHeaderTable.h"
#include "token.h"
#include<iostream>
#include <iomanip>

using namespace std;

map<string, InstructionInfo> Instruction::allInstructions = {
  // instrukcija za zaustavljanje programa
  {"halt", InstructionInfo(0x00, 0)},

  // instrukcija softverskog prekida
  {"int", InstructionInfo(0x10, 1)},

  // instrukcija povratka iz prekidne rutine
  {"iret", InstructionInfo(0x20, 0)},

  // instrukcija poziva potprograma
  {"call", InstructionInfo(0x30, 1)},

  // instrukcija povratka iz potprograma
  {"ret", InstructionInfo(0x40, 0)},

  // instrukcija skoka
  {"jmp", InstructionInfo(0x50, 1)},
  {"jeq", InstructionInfo(0x51, 1)},
  {"jne", InstructionInfo(0x52, 1)},
  {"jgt", InstructionInfo(0x53, 1)},

  // instrukcija atomicne zamene vrednosti
  {"xchg", InstructionInfo(0x60, 2)},

  // instrukcija aritmetickih operacija
  {"add", InstructionInfo(0x70, 2)},
  {"sub", InstructionInfo(0x71, 2)},
  {"mul", InstructionInfo(0x72, 2)},
  {"div", InstructionInfo(0x73, 2)},
  {"cmp", InstructionInfo(0x74, 2)},

  // instrukcija logickih operacija
  {"not", InstructionInfo(0x80, 1)},
  {"and", InstructionInfo(0x81, 2)},
  {"or", InstructionInfo(0x82, 2)},
  {"xor", InstructionInfo(0x83, 2)},
  {"test", InstructionInfo(0x84, 2)},

  // instrukcija pomerackih operacija
  {"shl", InstructionInfo(0x90, 2)},
  {"shr", InstructionInfo(0x91, 2)},

  // instrukcija ucitavanje podataka
  {"ldr", InstructionInfo(0xa0, 2)},

  //instrukcija smestanja podataka
  {"str", InstructionInfo(0xb0, 2)},

  //instrukcija za rad sa stekom ???
  {"push", InstructionInfo(0xb0, 1)}, //str
  {"pop", InstructionInfo(0xa0, 1)} //ldr
};

map<AddressingType, uint8_t> allAddressingModes = {
  {AddressingType::IMMEDIATE, 0x00},
  {AddressingType::REGISTER_DIRECT, 0x01},
  {AddressingType::REGISTER_INDIRECT, 0x02},
  {AddressingType::REGISTER_INDIRECT_WITH_SIGNED_OFFSET, 0x03},
  {AddressingType::MEMORY_DIRECT, 0x04},
  {AddressingType::REGISTER_DIRECT_WITH_SIGNED_OFFSET, 0x05}
};

map<UpdateType, uint8_t> allUpdateModes = {
  {UpdateType::NO_UPDATE, 0x00},
  {UpdateType::DEC_BY_2_BEFORE_FORMING_ADDRESS, 0x01},
  {UpdateType::INC_BY_2_BEFORE_FORMING_ADDRESS, 0x02},
  {UpdateType::DEC_BY_2_AFTER_FORMING_ADDRESS, 0x03},
  {UpdateType::INC_BY_2_AFTER_FORMING_ADDRESS, 0x04}
};

uint16_t Instruction::processInstruction(vector<sym_table_entry>& sym_table, string mnemonic, queue<string> operands, unsigned long lc, 
  string curr_section, unsigned long lineNo){
  
  vector<int8_t> instruction_code;

  // find instruction_info
  auto instruction = allInstructions.find(mnemonic);

  if(instruction == allInstructions.end())
    throw MyException("Instruction " + mnemonic + " does not exist. Error occurred at line " + to_string(lineNo + 1) + ".");
  /*if(instruction->second.numOfOperands != operands.size())
    throw MyException("Wrong syntax of instruction " + mnemonic + ". Error occurred at line " + to_string(lineNo + 1) + ".");*/

  // 1B (OC3 OC2 OC1 OC0 MOD3 MOD2 MOD1 MOD0)
  // push opcode
  instruction_code.push_back(instruction->second.opcode);

   // 1B instruction
  if(mnemonic == "halt" || mnemonic == "iret" || mnemonic == "ret"){
    
  }// 2B instruction with 1 operand && 3B instructions PUSH and POP
  else if(mnemonic == "int" || mnemonic == "not" || mnemonic == "push" ||mnemonic == "pop"){
    string operand = operands.front();
    operands.pop();

    uint8_t regD = stoi(operand.substr(1, operand.size()), NULL, 0);

    if(mnemonic == "push"){
      uint8_t byte_2 = (regD << 4) | 0x06; //SP
      instruction_code.push_back(byte_2);
      instruction_code.push_back((DEC_BY_2_BEFORE_FORMING_ADDRESS << 4) | REGISTER_INDIRECT);
    } 
    else if(mnemonic == "pop"){
      uint8_t byte_2 = (regD << 4) | 0x06; //SP
      instruction_code.push_back(byte_2);
      instruction_code.push_back((INC_BY_2_AFTER_FORMING_ADDRESS << 4) | REGISTER_INDIRECT);
    }
    else if(mnemonic == "not"){
      uint8_t byte_2 = (regD << 4) | 0x00;
      instruction_code.push_back(byte_2);
    }
    else {
      uint8_t byte_2 = (regD << 4) | 0x0f;
      instruction_code.push_back(byte_2);
    }
  }// 2B instruction with 2 operands
  else if(mnemonic == "xchg" || 
          mnemonic == "add" || mnemonic == "sub" || mnemonic == "mul" || mnemonic == "div" || mnemonic == "cmp" || 
          mnemonic == "and" || mnemonic == "or" || mnemonic == "xor" || mnemonic == "test" ||
          mnemonic == "shl" || mnemonic == "shr"){
  
  // 1. operand          
  string operand = operands.front();
  operands.pop();
  uint8_t regD = stoi(operand.substr(1, operand.size()), NULL, 0);
  // 2. operand 
  operand = operands.front();
  operands.pop();
  uint8_t regS = stoi(operand.substr(1, operand.size()), NULL, 0);

  uint8_t byte_2 = (regD << 4) | regS;          
  instruction_code.push_back(byte_2);  
  }// 3B or 5B instruction with 1 operand
  else if(mnemonic == "call" || mnemonic == "jmp" || mnemonic == "jeq" || mnemonic == "jne" || mnemonic == "jgt"){
    //TODO
    TokenType type = getOperandTokenType(operands);
    switch(type){
      case TokenType::DEC_OPERAND: 
      case TokenType::HEX_OPERAND:{
        // byte2
        instruction_code.push_back(0xf0);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::IMMEDIATE);
        // byte4
        int16_t data = stoi(operands.front(), nullptr, 0);
        int8_t data_high = (data >> 8) & 0xff;
        int8_t data_low = data & 0xff;
        instruction_code.push_back(data_high);
        // byte5
        instruction_code.push_back(data_low);
        
        break;
      }
      case TokenType::SYMBOL_OPERAND:{
        string operand = operands.front();

        // byte2
        instruction_code.push_back(0xf0);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::IMMEDIATE);
        // byte4
        instruction_code.push_back(0x00);
        // byte5
        instruction_code.push_back(0x00);

        processSymbol(lc + 3, relocation_type::R_X86_64_16, curr_section, operand);

        break;
      }
      case TokenType::SYMBOL_PC_REL:{
        string operand = operands.front().substr(1, operands.front().size());

        // byte2
        instruction_code.push_back(0xf7);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::REGISTER_DIRECT_WITH_SIGNED_OFFSET);
        // byte4
        instruction_code.push_back(0x00);
        // byte5
        instruction_code.push_back(0x00);

        processSymbol(lc + 3, relocation_type::R_X86_64_PC16, curr_section, operand);

        break;
      }
      case TokenType::MEM_DIR_DEC_JMP:
      case TokenType::MEM_DIR_HEX_JMP:{
        // byte2
        instruction_code.push_back(0xf0);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::MEMORY_DIRECT);
        // byte4
        int16_t data = stoi(operands.front().substr(1, operands.front().size()), nullptr, 0);
        int8_t data_high = (data >> 8) & 0xff;
        int8_t data_low = data & 0xff;
        instruction_code.push_back(data_high);
        // byte5
        instruction_code.push_back(data_low);
        
        break;
      }
      case TokenType::MEM_DIR_SYMBOL_JMP:{
        string operand = operands.front().substr(1, operands.front().size());

        // byte2
        instruction_code.push_back(0xf0);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::MEMORY_DIRECT);
        // byte4
        instruction_code.push_back(0x00);
        // byte5
        instruction_code.push_back(0x00);

        processSymbol(lc + 3, relocation_type::R_X86_64_16, curr_section, operand);

        break;
      }
      case TokenType::REG_DIR_JMP:{
        string operand = operands.front().substr(1, operands.front().size());

        // byte2
        uint8_t regS = stoi(operand.substr(1, operand.size()), NULL, 0);
        instruction_code.push_back((0xf << 4) | regS);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::REGISTER_DIRECT);
 
        break;
      }
      case TokenType::REG_INDIR_JMP:{
        string operand = operands.front().substr(2, operands.front().size() - 1);

        // byte2
        uint8_t regS = stoi(operand.substr(1, operand.size()), NULL, 0);
        instruction_code.push_back((0xf << 4) | regS);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::REGISTER_INDIRECT);
 
        break;
      }
      case TokenType::REG_INDIR_WITH_OFFSET_JMP:{
        string reg = operands.front().substr(2, operands.front().size());
        operands.pop();
        operands.pop();
        string offset = operands.front().substr(0, operands.front().size() - 1);

        // byte2
        uint8_t regS = stoi(reg.substr(1, reg.size()), NULL, 0);
        instruction_code.push_back((0xf << 4) | regS);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::REGISTER_INDIRECT_WITH_SIGNED_OFFSET);

        TokenType type = (Token::determineTokenType(offset)).getType();
        if(type == TokenType::HEX_OPERAND || type == TokenType::DEC_OPERAND){
          // byte4
          int16_t data = stoi(offset, nullptr, 0);
          int8_t data_high = (data >> 8) & 0xff;
          int8_t data_low = data & 0xff;
          instruction_code.push_back(data_high);
          // byte5
          instruction_code.push_back(data_low);
        }
        else if(type == TokenType::SYMBOL_OPERAND){
          // byte4
          instruction_code.push_back(0x00);
          // byte5
          instruction_code.push_back(0x00);

          processSymbol(lc + 3, relocation_type::R_X86_64_16, curr_section, offset);
        }

        break;
      }
    }
  }// 3B or 5B instruction with 2 operands
  else if(mnemonic == "ldr" || mnemonic == "str"){
    // TODO
    string reg = operands.front();
    operands.pop();
    uint8_t regD = stoi(reg.substr(1, reg.size()), NULL, 0);

    TokenType type = getOperandTokenType(operands);
    switch(type){
      case TokenType::IMM_HEX_DATA:
      case TokenType::IMM_DEC_DATA:{
        if(mnemonic == "str")
          throw MyException("Instruction 'str' cannot be used with immediate addressing. Error at line: " + to_string(lineNo + 1));
        // byte2
        instruction_code.push_back((regD << 4) | 0x0);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::IMMEDIATE);
        // byte4
        int16_t data = stoi(operands.front().substr(1, operands.front().size()), nullptr, 0);
        int8_t data_high = (data >> 8) & 0xff;
        int8_t data_low = data & 0xff;
        instruction_code.push_back(data_high);
        // byte5
        instruction_code.push_back(data_low);
        
        break;
      }
      case TokenType::IMM_SYMBOL_DATA:{
        if(mnemonic == "str")
          throw MyException("Instruction 'str' cannot be used with immediate addressing. Error at line: " + to_string(lineNo + 1));
        string operand = operands.front().substr(1, operands.front().size());
        // byte2
        instruction_code.push_back((regD << 4) | 0x0);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::IMMEDIATE);
        // byte4
        instruction_code.push_back(0x00);
        // byte5
        instruction_code.push_back(0x00);

        processSymbol(lc + 3, relocation_type::R_X86_64_16, curr_section, operand);
        break;
      }
      case TokenType::HEX_OPERAND:
      case TokenType::DEC_OPERAND:{
        // byte2
        instruction_code.push_back((regD << 4) | 0x0);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::MEMORY_DIRECT);
        // byte4
        int16_t data = stoi(operands.front(), nullptr, 0);
        uint8_t data_high = (data >> 8) & 0xff;
        uint8_t data_low = data & 0xff;
        instruction_code.push_back(data_high);
        // byte5
        instruction_code.push_back(data_low);
        
        break;
      }
      case TokenType::SYMBOL_OPERAND:{
        string operand = operands.front();

        // byte2
        instruction_code.push_back((regD << 4) | 0x0);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::MEMORY_DIRECT);
        // byte4
        instruction_code.push_back(0x00);
        // byte5
        instruction_code.push_back(0x00);

        processSymbol(lc + 3, relocation_type::R_X86_64_16, curr_section, operand);
        break;
      }
      case TokenType::SYMBOL_PC_REL:{
        string operand = operands.front().substr(1, operands.front().size());

        // byte2
        instruction_code.push_back((regD << 4) | 0x07);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::REGISTER_INDIRECT_WITH_SIGNED_OFFSET);
        // byte4
        instruction_code.push_back(0x00);
        // byte5
        instruction_code.push_back(0x00);

        processSymbol(lc + 3, relocation_type::R_X86_64_PC16, curr_section, operand);
        break;
      }
      case TokenType::REG_DIR_DATA:{
        string operand = operands.front();

        // byte2
        uint8_t regS = stoi(operand.substr(1, operand.size()), NULL, 0);
        instruction_code.push_back((regD << 4) | regS);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::REGISTER_DIRECT);
 
        break;
      }
      case TokenType::REG_INDIR_DATA:{
        string operand = operands.front().substr(1, (operands.front().size() - 1));

        // byte2
        uint8_t regS = stoi(operand.substr(1, operand.size()), NULL, 0);
        instruction_code.push_back((regD << 4) | regS);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::REGISTER_INDIRECT);
 
        break;
      }
      case TokenType::REG_INDIR_WITH_OFFSET_DATA:{
        string r = operands.front().substr(1, operands.front().size());
        operands.pop();
        operands.pop();
        string offset = operands.front().substr(0, operands.front().size() - 1);

        // byte2
        uint8_t regS = stoi(r.substr(1, r.size()), NULL, 0);
        instruction_code.push_back((regD << 4) | regS);
        // byte3
        instruction_code.push_back((UpdateType::NO_UPDATE << 4) | AddressingType::REGISTER_INDIRECT_WITH_SIGNED_OFFSET);

        TokenType t = (Token::determineTokenType(offset)).getType();
        if(t == TokenType::HEX_OPERAND || t == TokenType::DEC_OPERAND){
          // byte4
          int16_t data = stoi(offset, nullptr, 0);
          int8_t data_high = (data >> 8) & 0xff;
          int8_t data_low = data & 0xff;
          instruction_code.push_back(data_high);
          // byte5
          instruction_code.push_back(data_low);
        }
        else if(t == TokenType::SYMBOL_OPERAND){
          // byte4
          instruction_code.push_back(0x00);
          // byte5
          instruction_code.push_back(0x00);

          processSymbol(lc + 3, relocation_type::R_X86_64_16, curr_section, offset);
        }

        break;
      }
    }
  }

  // insert instruction_code into SectionTable .<section>
  for (int8_t byte : instruction_code)
	{
		SectionHeaderTable::getSectionTable(curr_section)->insertByte(byte);
	}

  return instruction_code.size();

  //test
  /*for(uint8_t i = 0; i < instruction_code.size(); i++){
    cout<<std::setfill('0')<<std::setw(2)<<hex<<static_cast<int>(instruction_code[i])<<" ";
  }
  cout<<endl;*/
}

TokenType Instruction::getOperandTokenType(queue<string> operands){
  string token = operands.front();
  operands.pop();
  Token operand = Token::determineTokenType(token);

  while(operand.getType() == TokenType::NONEXISTENT){
    token += operands.front();
    operands.pop();
    operand = Token::determineTokenType(token);
  }

  return operand.getType();
}

void Instruction::processSymbol(uint16_t lc, relocation_type type, string curr_section_name, string operand){
  
  vector<sym_table_entry>& sym_table = SectionHeaderTable::getSymbolTable()->sym_table;

  unsigned long i = SectionHeaderTable::getSymbolTable()->findSymbol(operand);
  if(i == 0){
    //symbol not in sym_table -> insert new symbol, make flink entry
    SectionHeaderTable::getSymbolTable()->insertSymbol(operand, 0, 0, bind_type::LOCAL, false);
    uint16_t index = SectionHeaderTable::getSymbolTable()->getSym_tableSize() - 1;
    flink flink_;
    if(type == relocation_type::R_X86_64_PC16){
      flink_ = flink(lc, curr_section_name, type, -2);
    }
    else {
      flink_ = flink(lc, curr_section_name);
    }
    sym_table[index].fwdref.push_back(flink_);
  }
  else if(i != 0 && sym_table[i].defined == false){
    //symbol in sym_table but not defined -> make flink entry
    flink flink_;
    if(type == relocation_type::R_X86_64_PC16){
      flink_ = flink(lc, curr_section_name, type, -2);
    }
    else {
      flink_ = flink(lc, curr_section_name);
    }
    sym_table[i].fwdref.push_back(flink_);
  } else if(i != 0 && sym_table[i].defined == true){
    //symbol in sym_table and is defined -> make relocation entry
    if(sym_table[i].bind == bind_type::EXTERN || sym_table[i].bind == bind_type::GLOBAL){
      if(type == relocation_type::R_X86_64_PC16){
        SectionHeaderTable::getRelocationTable(curr_section_name)->createEntry(lc, type, i, -2);
      }
      else {
        SectionHeaderTable::getRelocationTable(curr_section_name)->createEntry(lc, type, i, 0);
      }
    }
    else{
      if(type == relocation_type::R_X86_64_PC16){
        SectionHeaderTable::getRelocationTable(curr_section_name)->
          createEntry(lc, type, sym_table[i].section, sym_table[i].value - 2);
      }
      else {
        SectionHeaderTable::getRelocationTable(curr_section_name)->
          createEntry(lc, type, sym_table[i].section, sym_table[i].value);
      }
    }
  }
}