#ifndef BINARYPRINTER
#define BINARYPRINTER

#include <fstream>
#include <vector>
#include <map>

#include "sectionTable.h"
#include "symbolTable.h"
#include "relocationTable.h"

using namespace std;

class BinaryPrinter{
  private:
    vector<SectionTable*> sections;
    vector<sym_table_entry> symbols;
    map<string, vector<rel_table_entry>> relocations;

  public:
    void printToOutputFileBinary(string output_file);
};

#endif