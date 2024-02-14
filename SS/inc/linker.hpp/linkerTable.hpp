#ifndef LINKERTABLE_HPP
#define LINKERTABLE_HPP

#include <map>
#include <vector>
#include "dataTable.hpp"

class LinkerTable{
  private:
    DataTable* linker_table;
    vector<DataTable*>& data_tables;
    vector<unsigned char> combined_data;

  public:
    LinkerTable(vector<DataTable*>& data_tables_): data_tables(data_tables_), linker_table(new DataTable()){}

    void makeLinkerTable();

    void addSections();

    void addSymbols();

    void addRelocations();

    void mergeSections();

    Section* findSection(string name);

    Symbol* findSymbol(string name);

    int getTableSize();

    void checkSymbols();

    void writeToOutputFile(string output_file);

    void printLinkerTable();
};

#endif