#ifndef DATATABLE_HPP
#define DATATABLE_HPP

#include <map>
#include <vector>
#include "section.hpp"
#include "symbol.hpp"
#include "relocation.hpp"

class DataTable
{
private:
  vector<Section *> sections;
  vector<Symbol *> symbols;
  vector<Relocation *> relocations;

public:
  void addSection(Section *section);

  void addSymbol(Symbol *symbol);

  void addRelocation(Relocation *relocation);

  vector<Section *> getSections(){ return sections; }

  vector<Symbol *> getSymbols() { return symbols; }

  vector<Relocation *> getRelocations() {return relocations; }

  Section* findSection(string name);

  Symbol* findSymbol(string name);
};

#endif