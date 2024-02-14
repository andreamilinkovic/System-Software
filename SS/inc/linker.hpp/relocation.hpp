#ifndef RELOCATION_HPP
#define RELOCATION_HPP

#include <iostream>

using namespace std;

enum relocation_type{
  R_X86_64_16, // abs
  R_X86_64_PC16, // PC rel
  L_ENDIAN
};

class Relocation{
  private:
    int offset;
    relocation_type type;
    string section;
    string symbol;
    int addend;

  public:
    Relocation(int offset_, int type_, string section_, string symbol_, int addend_): 
      offset(offset_), type((relocation_type)type_), section(section_), symbol(symbol_), addend(addend_){}

    int getOffset(){ return offset; }
    relocation_type getType(){ return type; }
    string getSection(){ return section; }
    string getSymbol(){ return symbol; }
    int getAddend(){ return addend; }
};

#endif