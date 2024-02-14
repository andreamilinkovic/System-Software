#include "binaryPrinter.h"
#include "sectionHeaderTable.h"
#include "myexception.h"

void BinaryPrinter::printToOutputFileBinary(string output_file){
  fstream outputFile;
  outputFile.open(output_file, ios::out);

  if (!outputFile.is_open())
	{
		throw MyException("Unable to open output file.");
	}

  sections = SectionHeaderTable::getSections();
  symbols = SectionHeaderTable::getSymbols();
  relocations = SectionHeaderTable::getRelocations();

  // num of sections
  outputFile << sections.size() << endl;
  // about section
  for(SectionTable* section : sections){
    outputFile << section->sect_table.size() << " " << section->offset << " " << section->name << endl;
    for(int8_t byte : section->sect_table)
      outputFile << ((int)byte & 0xFF) << " ";
    outputFile << endl;
  }
  outputFile << endl;

  // num of symbols
  outputFile << symbols.size() << endl;
  // about symbols
  for(unsigned long i = 0; i < symbols.size(); i++){
    outputFile << symbols[i].name << " " << SectionHeaderTable::getSymbolTable()->findSymbolName(symbols[i].section) << " " << symbols[i].value << " ";
    switch(symbols[i].bind){
      case bind_type::LOCAL:{
        outputFile << "SECT";
        break;
      }
      case bind_type::GLOBAL:{
        outputFile << "GLO";
        break;
      }
      case bind_type::EXTERN:{
        outputFile << "EXT";
        break;
      }
    }
  outputFile << endl;
  }
  outputFile << endl;

  // num of relocations
  int numOfRelocations = 0;
  for(auto r : relocations)
    numOfRelocations += r.second.size();

  outputFile << numOfRelocations << endl;
  // about relocations
  for(auto r : relocations){
    for(unsigned long i = 0; i < r.second.size(); i++){
      outputFile << r.second[i].offset << " " << r.second[i].type << " " << r.first << " " <<
        SectionHeaderTable::getSymbolTable()->findSymbolName(r.second[i].symbol) << " " << r.second[i].addend << endl;
    }
  }

  outputFile.close();
}