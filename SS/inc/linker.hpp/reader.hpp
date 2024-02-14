#ifndef READER_HPP
#define READER_HPP

#include <vector>
#include <iostream>
#include "dataTable.hpp"

using namespace std;

class Reader{
  public:
    DataTable* readInputFile(string input_file);

    void readSections(fstream& inputFile, DataTable* dt);

    void readSymbols(fstream& inputFile, DataTable* dt);

    void readRelocations(fstream& inputFile, DataTable* dt);
};

#endif