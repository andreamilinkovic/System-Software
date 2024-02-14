#ifndef SYMBOLTABLE_
#define SYMBOLTABLE_

#include<iostream>
#include<vector>

#include "relocationTable.h"

using namespace std;

enum sym_type{
  NOTYPE,
  SECT
};

enum bind_type{
  LOCAL,
  GLOBAL,
  EXTERN
};

struct flink{
  uint16_t offset; //address of unresolved symbol in section
  string section; //section of unresolved symbol  

  //unresolved symbol can be resolved only if it's defined in the same section
  //if symbol cannot be resolved before linker
  //bool symbol_resolved;
  relocation_type type;
  int16_t addend;

  flink() : section(""){};

  flink(uint16_t offset_, string section_, 
    relocation_type type_ = relocation_type::R_X86_64_16, int16_t addend_ = 0){

      offset = offset_;
      section = section_;

      type = type_;
      addend = addend_;
  }
};

struct sym_table_entry{
  uint16_t value;
  uint16_t size;
  sym_type type;
  bind_type bind;
  unsigned long section;
  string name;
  bool defined;
  vector<flink> fwdref;

  sym_table_entry(string name_, uint16_t value_, unsigned long section_, 
    bind_type bind_, bool defined_, sym_type type_ = sym_type::NOTYPE, flink flink_ = flink()){

      name = name_;
      value = value_;
      section = section_;
      size = 0;
      bind = bind_;
      defined = defined_;
      type = type_;

      if(flink_.section != ""){
        fwdref.push_back(flink_);
      }
  }
};

class SymbolTable{
  protected:
    vector<sym_table_entry> sym_table;

    friend class Assembler;

    friend class Instruction;

  public:
    SymbolTable();

    uint16_t getSym_tableSize();

    void updateSymbolSize(uint16_t size, string name);

    void insertSymbol(string name, uint16_t lc, unsigned long section, bind_type bind, bool defined);

    unsigned long findSymbol(string name); //returns index of symbol in sym_table

    string findSymbolName(unsigned long section);

    vector<sym_table_entry> getSymbols();

    void writeTableToOutputFile(fstream& outputFile);
};

#endif