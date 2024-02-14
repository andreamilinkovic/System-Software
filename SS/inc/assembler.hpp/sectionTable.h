#ifndef SECTIONTABLE_
#define SECTIONTABLE_

#include<iostream>
#include<vector>

using namespace std;

class SectionTable{
  private:
    string name;
    vector<int8_t> sect_table; //binary data
    uint16_t offset;

    friend class BinaryPrinter;

  public:
    SectionTable(string name_): name(name_), offset(0){}

    void insertByte(int8_t byte);

    void insertWord(int16_t word);

    void insertWordtoAddress(int16_t word, uint16_t address);

    void writeTableToOutputFile(fstream& outputFile);
};

#endif