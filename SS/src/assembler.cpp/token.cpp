#include<iostream>

#include "token.h"

using namespace std;

Token Token::determineTokenType(string token){
  for(unsigned long i = 0; i < patternNum; i++){
    if(regex_match(token, RegexPattern[i])){
      switch(i){
        case 0:{
          return Token(token.substr(0, token.size()-1), TokenType::LABEL);
        }
        case 1:{
          return Token(token, TokenType::INSTRUCTION);
        }
        case 2:{
          return Token(token, TokenType::ACCESS_RIGHTS_DIRECTIVE);
        }
        case 3:{
          return Token(token, TokenType::MEMORY_ALLOCATION_DIRECTIVE);
        }
        case 4:{
          return Token(token, TokenType::SECTION_DIRECTIVE);
        }
        case 5:{
          return Token(token, TokenType::END_DIRECTIVE);
        }
        case 6:{
          return Token(token, TokenType::ASCII_DIRECTIVE);
        }
        case 7:{
          return Token(token, TokenType::EQU_DIRECTIVE);
        }
        case 8:{
          return Token(token, TokenType::IMM_HEX_DATA);
        }
        case 9:{
          return Token(token, TokenType::IMM_DEC_DATA);
        }
        case 10:{
          return Token(token, TokenType::IMM_SYMBOL_DATA);
        }
        case 11:{
          token = regex_replace(token, regex("sp"), "r6");
          token = regex_replace(token, regex("pc"), "r7");
          return Token(token, TokenType::REG_DIR_DATA);
        }
        case 12:{
          token = regex_replace(token, regex("sp"), "r6");
          token = regex_replace(token, regex("pc"), "r7");
          return Token(token, TokenType::REG_INDIR_DATA);
        }
        case 13:{
          token = regex_replace(token, regex("sp"), "r6");
          token = regex_replace(token, regex("pc"), "r7");
          return Token(token, TokenType::REG_INDIR_WITH_OFFSET_DATA);
        }
        case 14:{
          return Token(token, TokenType::HEX_OPERAND);
        }
        case 15:{
          return Token(token, TokenType::DEC_OPERAND);
        }
        case 16:{
          return Token(token, TokenType::SYMBOL_OPERAND);
        }
        case 17:{
          return Token(token, TokenType::SYMBOL_PC_REL);
        }
        case 18:{
          return Token(token, TokenType::MEM_DIR_HEX_JMP);
        }
        case 19:{
          return Token(token, TokenType::MEM_DIR_DEC_JMP);
        }
        case 20:{
          token = regex_replace(token, regex("sp"), "r6");
          token = regex_replace(token, regex("pc"), "r7");
          return Token(token, TokenType::REG_DIR_JMP);
        }
        case 21:{
          return Token(token, TokenType::MEM_DIR_SYMBOL_JMP);
        }
        case 22:{
          token = regex_replace(token, regex("sp"), "r6");
          token = regex_replace(token, regex("pc"), "r7");
          return Token(token, TokenType::REG_INDIR_JMP);
        }
        case 23:{
          token = regex_replace(token, regex("sp"), "r6");
          token = regex_replace(token, regex("pc"), "r7");
          return Token(token, TokenType::REG_INDIR_WITH_OFFSET_JMP);
        }
      }
    }
  }

  return Token("", TokenType::NONEXISTENT);
}