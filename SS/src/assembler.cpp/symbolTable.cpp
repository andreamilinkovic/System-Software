#include "sectionHeaderTable.h"
#include "symbolTable.h"
#include "myexception.h"

#include<iomanip>
#include<iostream>
#include<fstream>

using namespace std;

SymbolTable::SymbolTable(){
  sym_table.push_back(sym_table_entry("UND", 0, 0, bind_type::LOCAL, false)); //UND section symbol
}

uint16_t SymbolTable::getSym_tableSize(){
  return sym_table.size();
}

void SymbolTable::updateSymbolSize(uint16_t size, string name){
  unsigned long index = findSymbol(name);
  sym_table[index].size = size;
}

void SymbolTable::insertSymbol(string name, uint16_t lc, unsigned long section, 
  bind_type bind, bool defined){

    sym_table_entry entry = sym_table_entry(name, lc, section, bind, defined);
    sym_table.push_back(entry);
}

unsigned long SymbolTable::findSymbol(string name){
  unsigned long i = 0;
  for(sym_table_entry entry : sym_table){
      if(entry.name == name)
        return i;
      i++;
  }
  return 0; // und entry
}

string SymbolTable::findSymbolName(unsigned long section){
	return sym_table[section].name;
}

vector<sym_table_entry> SymbolTable::getSymbols(){
	vector<sym_table_entry> symbols;

	for(sym_table_entry entry : sym_table){
		if(entry.bind == bind_type::LOCAL && entry.type != sym_type::SECT)
			continue;
		
		symbols.push_back(entry);
	}

	return symbols;
}

void SymbolTable::writeTableToOutputFile(fstream& outputFile){
	const unsigned short w = 15;
	outputFile<<endl<<"SYMBOL TABLE: "<<endl<<endl;
	outputFile<<std::left;
	outputFile<<setw(w)<<"Index";
	outputFile<<setw(w)<<"Name";
	outputFile<<setw(w)<<"Value";
	outputFile<<setw(w)<<"Section";
	outputFile<<setw(w)<<"Size";
	outputFile<<setw(w)<<"Bind";
	outputFile<<setw(w)<<"Defined";
	outputFile<<setw(w)<<"Flink";
	outputFile<<endl;

	int i = 0;
	for (sym_table_entry entry : sym_table)
	{
    //index
		outputFile<<setw(w)<<to_string(i);

    //name
		if (i == 0)
			outputFile<<setw(w)<<"UND";
		else
			outputFile<<setw(w)<<entry.name;

    //value
		if(entry.defined){
      stringstream ss;
		  ss<<left<<"0x"<<hex<<entry.value;
		  outputFile<<setw(w)<<ss.str();
    }
		else
			outputFile<<setw(w)<<"n/a";

    //section
		outputFile<<setw(w)<<to_string(entry.section);

    //size
    stringstream ss;
		ss<<left<<"0x"<<hex<<entry.size;
		outputFile<<setw(w)<<ss.str();
    
    //bind
		switch (entry.bind)
		{
		case bind_type::LOCAL:
			outputFile<<setw(w)<<"LOC";
			break;
		case bind_type::GLOBAL:
			outputFile<<setw(w)<<"GLO";
			break;
		case bind_type::EXTERN:
			outputFile<<setw(w)<<"EXT";
			break;
		}

    //defined
		if (entry.defined)
			outputFile<<setw(w)<<"true";
		else
			outputFile<<setw(w)<<"false";

    //fwdref
		if (!entry.fwdref.empty())
		{
			for(flink fl : entry.fwdref){
        stringstream ss;
		    ss<<left<<"0x"<<hex<<setw(2)<<fl.offset;
		    outputFile<<setw(w)<<ss.str() + "/" + fl.section;
      }
		}
		
		outputFile<<endl;
		i++;
	}
	outputFile<<endl<<endl;
}