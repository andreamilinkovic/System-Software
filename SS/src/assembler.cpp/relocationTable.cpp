#include<stdint.h>
#include<iomanip>
#include<iostream>
#include<fstream>

#include "relocationTable.h"

using namespace std;

void RelocationTable::createEntry(uint16_t offset, relocation_type type, unsigned long symbol, int16_t addend){
  rel_table.push_back(rel_table_entry(offset, type, symbol, addend));
}

vector<rel_table_entry> RelocationTable::getRelocations(){
	vector<rel_table_entry> relocations;

	for(rel_table_entry entry : rel_table){
		relocations.push_back(entry);
	}

	return relocations;
}

void RelocationTable::writeTableToOutputFile(fstream& outputFile){
	const unsigned short w = 20;
	outputFile<<endl<<"RELOCATION TABLE " + name <<endl<<endl;
	outputFile<<std::left;
	outputFile<<setw(w)<<"Offset";
	outputFile<<setw(w)<<"Type";
	outputFile<<setw(w)<<"Symbol";
	outputFile<<setw(w)<<"Addend";
	outputFile<<endl;

	for (rel_table_entry entry : rel_table)
	{
		stringstream ss;
		ss<<left<<"0x"<<hex<<entry.offset;
		outputFile<<setw(w)<<ss.str();
		switch (entry.type)
		{
			case relocation_type::R_X86_64_16:{
				outputFile<<setw(w)<<"R_X86_64_16";
				break;
			}
			case relocation_type::R_X86_64_PC16:{
				outputFile<<setw(w)<<"R_X86_64_PC16";
				break;
			}
		}
		outputFile<<setw(w)<<to_string(entry.symbol);
		outputFile<<setw(w)<<entry.addend;
		outputFile<<endl;
	}
	outputFile<<endl;
}