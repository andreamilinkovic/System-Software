#ifndef TOKEN_H
#define TOKEN_H

#include<regex>

using namespace std;

const unsigned long patternNum = 24;

const regex RegexPattern[patternNum] = {
  //label -> LABEL
  regex("^([A-Za-z_][A-Za-z0-9_]*):$"), 

  //instructions -> INSTRUCTION
  regex("^(halt|int|iret|call|ret|jmp|jeq|jne|jgt|push|pop|xchg|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|ldr|str)$"),
  
  //derectives
  regex("^\\.(global|extern)$"), //.global, .extern -> ACCESS_RIGHTS_DIRECTIVE
  regex("^\\.(word|skip)$"), //.word, .skip -> MEMORY_ALLOCATION_DIRECTIVE
  regex("^\\.section$"), //.section -> SECTION_DIRECTIVE
  regex("^\\.end$"), //.end -> END_DIRECTIVE
  regex("^\\.ascii$"), //.ascii -> ASCII_DIRECTIVE
  regex("^\\.equ$"), //.equ -> EQU_DIRECTIVE
  
  //operand type -> data
  regex("^\\$0x[0-9a-fA-F]{1,}$"), //IMM_HEX_DATA
  regex("^\\$\\-{0,1}[0-9]{1,}$"), //IMM_DEC_DATA
  regex("^\\$([A-Za-z_][A-Za-z0-9_]*)$"), //IMM_SYMBOL_DATA  
  regex("^(r[0-7]|sp|pc|psw)$"), //REG_DIR_DATA
  regex("^\\[(r[0-7]|sp|pc|psw)\\]$"), //REG_INDIR_DATA
  regex("^\\[(r[0-7]|sp|pc|psw)(\\-|\\+)[a-zA-Z0-9_]{1,}\\]$"), //REG_INDIR_WITH_OFFSET_DATA

  //data && jmp
  regex("^0x[0-9a-fA-F]{1,}$"), //HEX_OPERAND
  regex("^\\-{0,1}[0-9]{1,}$"), //DEC_OPERAND
  regex("^([A-Za-z_][A-Za-z0-9_]*)$"), //SYMBOL_OPERAND
  regex("^%([A-Za-z_][A-Za-z0-9_]*)$"), //SYMBOL_PC_REL

  //operand type -> jmp
  regex("^\\*0x[0-9a-fA-F]{1,}$"), //MEM_DIR_HEX_JMP
  regex("^\\*\\-{0,1}[0-9]{1,}$"), //MEM_DIR_DEC_JMP
  regex("^\\*(r[0-7]|sp|pc|psw)$"), //REG_DIR_JMP
  regex("^\\*([A-Za-z_][A-Za-z0-9_]*)$"), //MEM_DIR_SYMBOL_JMP
  regex("^\\*\\[(r[0-7]|sp|pc|psw)\\]$"), //REG_INDIR_JMP
  regex("^\\*\\[(r[0-7]|sp|pc|psw)(\\-|\\+)[a-zA-Z0-9_]{1,}\\]$"), //REG_INDIR_WITH_OFFSET_JMP
};

enum TokenType {
  NONEXISTENT = -1,

  LABEL = 0,
  INSTRUCTION = 1,

  ACCESS_RIGHTS_DIRECTIVE = 2,
  MEMORY_ALLOCATION_DIRECTIVE = 3,
  SECTION_DIRECTIVE = 4,
  END_DIRECTIVE = 5,
  ASCII_DIRECTIVE = 6,
  EQU_DIRECTIVE = 7,

  //addressing
  IMM_HEX_DATA = 8,
  IMM_DEC_DATA = 9,
  IMM_SYMBOL_DATA = 10,
  REG_DIR_DATA = 11,
  REG_INDIR_DATA = 12,
  REG_INDIR_WITH_OFFSET_DATA = 13,

  HEX_OPERAND = 14,
  DEC_OPERAND = 15,
  SYMBOL_OPERAND = 16,
  SYMBOL_PC_REL = 17,
  
  MEM_DIR_HEX_JMP = 18,
  MEM_DIR_DEC_JMP = 19,
  REG_DIR_JMP = 20,
  MEM_DIR_SYMBOL_JMP = 21,
  REG_INDIR_JMP = 22,
  REG_INDIR_WITH_OFFSET_JMP = 23,
};

class Token{
  private:
    string value;
    TokenType type;

  public:
    Token(string value_, TokenType type_) : value(value_), type(type_){}

    string getValue(){ return value; }
    TokenType getType() { return type; }
    
    static Token determineTokenType(string token);
};

#endif