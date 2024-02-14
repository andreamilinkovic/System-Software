#include "dataTable.hpp"

void DataTable::addSection(Section* section){
  sections.push_back(section);
}

void DataTable::addSymbol(Symbol* symbol){
  symbols.push_back(symbol);
}

void DataTable::addRelocation(Relocation* relocation){
  relocations.push_back(relocation);
}

Section* DataTable::findSection(string name){
  for(Section* s : sections){
    if(s->getName() == name)
      return s;
  }

  return nullptr;
}

Symbol* DataTable::findSymbol(string name){
  for(Symbol* s : symbols){
    if(s->getName() == name)
      return s;
  }

  return nullptr;
}