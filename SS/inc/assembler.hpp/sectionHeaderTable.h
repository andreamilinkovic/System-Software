#ifndef SECTIONHEADERTABLE_
#define SECTIONHEADERTABLE_

#include<iostream>
#include<vector>
#include <map>

#include "symbolTable.h"
#include "relocationTable.h"
#include "sectionTable.h"

using namespace std;

enum section_header_type{
  SHT_NULL, //null
	SHT_SYMTAB, //sym_table
	SHT_REL, //rel_table
	SHT_PROGBITS,	//section with content
	SHT_NOBITS, //section without content
};

struct sh_table_entry{
  string name;
  section_header_type type;

  uint16_t address;
  uint16_t size;

  SymbolTable* sym_table_ptr;
  RelocationTable* rel_table_ptr;
  SectionTable* sect_table_ptr;

  sh_table_entry(string name_, section_header_type type_, uint16_t address_, uint16_t size_):
    name(name_), type(type_), address(address_), size(size_) 
    {
      sym_table_ptr = nullptr;
      rel_table_ptr = nullptr;
      sect_table_ptr = nullptr;
    }
};

class SectionHeaderTable{
  private:
    static vector<sh_table_entry> sh_table;

  public:
    static SymbolTable* getSymbolTable();
    
    static RelocationTable* getRelocationTable(string name);

    static SectionTable* getSectionTable(string name);

    // for linker
    static vector<SectionTable*> getSections();

    static vector<sym_table_entry> getSymbols();

    static map<string, vector<rel_table_entry>> getRelocations();


    static void createEntry(string name, section_header_type type);

    static void writeSHTableToOutputFile(fstream& outputFile);
};

#endif