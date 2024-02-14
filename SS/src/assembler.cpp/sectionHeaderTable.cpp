#include "sectionHeaderTable.h"
#include<iostream>
#include<fstream>
#include <iomanip>

using namespace std;

vector<sh_table_entry> SectionHeaderTable::sh_table = {
	sh_table_entry("", SHT_NULL, 0, 0)
};

void SectionHeaderTable::createEntry(string name, section_header_type type){
  switch (type){
    case section_header_type::SHT_NULL:{
      sh_table.push_back(sh_table_entry(name, type, 0, 0));
      break;
    }
    case section_header_type::SHT_SYMTAB:{
      sh_table_entry entry = sh_table_entry(name, type, 0, 0);
      entry.sym_table_ptr = new SymbolTable();
      sh_table.push_back(entry);
      break;
    }
    case section_header_type::SHT_REL:{
      sh_table_entry entry = sh_table_entry(name, type, 0, 0);
      entry.rel_table_ptr = new RelocationTable(name);
      sh_table.push_back(entry);
      break;
    }
    case section_header_type::SHT_PROGBITS:{
      sh_table_entry entry = sh_table_entry(name, type, 0, 0);
      entry.sect_table_ptr = new SectionTable(name);
      sh_table.push_back(entry);
      break;
    }
    case section_header_type::SHT_NOBITS:{
      sh_table_entry entry = sh_table_entry(name, type, 0, 0);
      entry.sect_table_ptr = new SectionTable(name);
      sh_table.push_back(entry);
      break;
    }
    default:
      break;
  }
}

SymbolTable* SectionHeaderTable::getSymbolTable(){
  for(sh_table_entry entry : sh_table){
    if(entry.name == "sym_table")
      return entry.sym_table_ptr;
  }

  createEntry("sym_table", section_header_type::SHT_SYMTAB);
  return sh_table.back().sym_table_ptr;
}

RelocationTable* SectionHeaderTable::getRelocationTable(string name){
  name = ".rela." + name;
  
  for(sh_table_entry entry : sh_table){
    if(entry.name == name)
      return entry.rel_table_ptr;
  }

  createEntry(name, section_header_type::SHT_REL);
  return sh_table.back().rel_table_ptr;
}

SectionTable* SectionHeaderTable::getSectionTable(string name){
  name = "." + name;
  
  for(sh_table_entry entry : sh_table){
    if(entry.name == name)
      return entry.sect_table_ptr;
  }

  createEntry(name, section_header_type::SHT_PROGBITS);
  return sh_table.back().sect_table_ptr;
}

vector<SectionTable*> SectionHeaderTable::getSections(){
  vector<SectionTable*> sections;

  for(sh_table_entry entry : sh_table){
    if(entry.type == section_header_type::SHT_PROGBITS)
      sections.push_back(entry.sect_table_ptr);
  }

  return sections;
}

vector<sym_table_entry> SectionHeaderTable::getSymbols(){
  vector<sym_table_entry> symbols;

  SymbolTable* sym_table = getSymbolTable();
  symbols = sym_table->getSymbols();

  return symbols;
}

map<string, vector<rel_table_entry>> SectionHeaderTable::getRelocations(){
  vector<rel_table_entry> relocations;
  map<string, vector<rel_table_entry>> relocations_map;

  for(sh_table_entry entry : sh_table){
    relocations.clear();
    if(entry.type == section_header_type::SHT_REL){
      vector<rel_table_entry> rel_table_entries = entry.rel_table_ptr->getRelocations();
      
      for(rel_table_entry e : rel_table_entries)
        relocations.push_back(e);

      string section_str = entry.name.substr(5, entry.name.length() - 1);

      relocations_map.insert(pair<string, vector<rel_table_entry>>(section_str, relocations));
    }
  }

  return relocations_map;
}

void SectionHeaderTable::writeSHTableToOutputFile(fstream& outputFile){
  const unsigned short w = 20;
  outputFile<<endl<<"SECTION HEADER TABLE:"<<endl<<endl;
  outputFile<<std::left;
	outputFile<<setw(w)<<"Index";
	outputFile<<setw(w)<<"Name";
	outputFile<<setw(w)<<"Type";
	outputFile<<setw(w)<<"Address";
	outputFile<<endl;

  int i = 0;
  for(sh_table_entry entry : sh_table){
    outputFile<<setw(w)<<to_string(i);
    outputFile<<setw(w)<<entry.name;
    switch(entry.type){
      case 0:{
        outputFile<<setw(w)<<SHT_NULL;
        break;
      }
      case 1:{
        outputFile<<setw(w)<<SHT_SYMTAB;
        break;
      }
      case 2:{
        outputFile<<setw(w)<<SHT_REL;
        break;
      }
      case 3:{
        outputFile<<setw(w)<<SHT_PROGBITS;
        break;
      }
      case 4:{
        outputFile<<setw(w)<<SHT_NOBITS;
        break;
      }
    }
    stringstream ss;
    ss<<left<<"0x"<<hex<<entry.address;
    outputFile<<setw(w)<<ss.str();
    outputFile<<endl;
    i++;
  }

  //sym_table
  for(sh_table_entry entry : sh_table){
    if(entry.sym_table_ptr != nullptr)
      entry.sym_table_ptr->writeTableToOutputFile(outputFile);
  }
  //rel_table
  for(sh_table_entry entry : sh_table){
    if(entry.rel_table_ptr != nullptr)
      entry.rel_table_ptr->writeTableToOutputFile(outputFile);
  }
  //sect_table
  for(sh_table_entry entry : sh_table){
    if(entry.sect_table_ptr != nullptr)
      entry.sect_table_ptr->writeTableToOutputFile(outputFile);
  }
}