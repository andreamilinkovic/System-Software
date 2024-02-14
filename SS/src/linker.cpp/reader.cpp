#include <fstream>
#include <vector>
#include <cstdint>
#include "reader.hpp"
#include "dataTable.hpp"
#include "myException.hpp"
#include "relocation.hpp"

DataTable* Reader::readInputFile(string input_file){
  DataTable* dt = new DataTable();

  fstream inputFile;
  inputFile.open(input_file, ios::in);
  if(!inputFile.is_open())
    throw MyException("Input file cannot be opened.");

  readSections(inputFile, dt);   

  readSymbols(inputFile, dt);

  readRelocations(inputFile, dt);

  return dt;
}

void Reader::readSections(fstream& inputFile, DataTable* dt){
  int numOfSections;
  inputFile >> numOfSections;
  
  int size;
  int address;
  string name;

  for(int i = 0; i < numOfSections; i++){
    inputFile >> size >> address >> name;
    vector<unsigned char> data;
    for(int i = 0; i < size; i++){
      int byte;
      inputFile >> byte;
      data.push_back((unsigned char)(byte & 0xff));
    }
    Section* section = new Section(name, size, address);
    section->setData(data);
    dt->addSection(section);

    /*cout << "size: " << size << endl;
    cout << "address: " << address << endl;
    cout << "name: " << name << endl;
    for(int i = 0; i < size; i++)
      cout << (int)data[i] << " ";
    cout << endl;*/
  }
}

void Reader::readSymbols(fstream& inputFile, DataTable* dt){
  int numOfSymbols;
  inputFile >> numOfSymbols;

  string name;
  string section;
  int value;
  string bind;

  for(unsigned long i = 0; i < numOfSymbols; i++){
    inputFile >> name >> section >> value >> bind;
    Symbol* symbol = new Symbol(name, section, value, bind);
    dt->addSymbol(symbol);
  
    /*cout << "name: " << name << endl;
    cout << "section: " << section << endl;
    cout << "value: " << value << endl;
    cout << "bind: " << bind << endl;*/
  }
}

void Reader::readRelocations(fstream& inputFile, DataTable* dt){
  int numOfRelocations;
  inputFile >> numOfRelocations;

  int offset;
  int type;
  string section;
  string symbol;
  int addend;

  for(unsigned long i = 0; i < numOfRelocations; i++){
    inputFile >> offset >> type >> section >> symbol >> addend;
    Relocation* relocation = new Relocation(offset, type, section, symbol, addend);
    dt->addRelocation(relocation);
  }

  /*for(int i = 0; i < dt->relocations.size(); i++){
    cout << "offset: " << dt->relocations[i]->offset << endl;
    cout << "type: " << dt->relocations[i]->type << endl;
    cout << "section: " << dt->relocations[i]->section << endl;
    cout << "symbol: " << dt->relocations[i]->symbol << endl;
    cout << "addend: " << dt->relocations[i]->addend << endl;
    cout << endl;
  }*/
}