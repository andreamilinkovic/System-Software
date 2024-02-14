#include "sectionTable.h"
#include<stdint.h>
#include<iomanip>
#include<iostream>
#include<fstream>

using namespace std;

void SectionTable::insertByte(int8_t byte){
  sect_table.push_back(byte);
}

void SectionTable::insertWord(int16_t word){
  int8_t data_low = word & 0x00ff;
  int8_t data_high = (word & 0xff00) >> 8;

  sect_table.push_back(data_low);
  sect_table.push_back(data_high);
}

void SectionTable::insertWordtoAddress(int16_t word, uint16_t address){
	for(uint16_t i = 0; i < sect_table.size(); i++){
		if(address == i){

			sect_table[i] = word;
			break;
		}
	}
}

void SectionTable::writeTableToOutputFile(fstream& outputFile){
if (name != ".bss")
	{
		const unsigned short w = 20;
		outputFile<<endl<<"SECTION " + name <<endl<<endl;

		const unsigned long max_bytes = 32;
		unsigned long cnt = 0;
		for (int8_t byte : sect_table)
		{
			if (cnt == max_bytes)
			{
				outputFile<<endl;
				cnt = 0;
			}
      stringstream ss;
      ss<<hex<<((byte >> 4) & 0xf);
	    ss<<hex<<(byte & 0xf);
      outputFile<<setw(2)<<ss.str()<<" ";
			cnt++;
		}
		outputFile<<endl<<endl;
	}
}