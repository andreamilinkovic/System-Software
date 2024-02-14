#ifndef RELOCATIONTABLE_
#define RELOCATIONTABLE_

#include<iostream>
#include<vector>

using namespace std;

enum relocation_type{
  R_X86_64_16,
  R_X86_64_PC16,
  L_ENDIAN
};

struct rel_table_entry{
  uint16_t offset;
  relocation_type type;
  unsigned long symbol;
  int16_t addend;

  rel_table_entry(uint16_t offset_, relocation_type type_, unsigned long symbol_, int16_t addend_): 
    offset(offset_), type(type_), symbol(symbol_), addend(addend_){}
};

class RelocationTable{
  private:
    string name;
    vector<rel_table_entry> rel_table;

  public:
    RelocationTable(string name_): name(name_){}

    void createEntry(uint16_t offset, relocation_type type, unsigned long symbol, int16_t addend);

    vector<rel_table_entry> getRelocations();

    void writeTableToOutputFile(fstream& outputFile);
};

#endif