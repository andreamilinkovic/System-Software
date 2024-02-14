#include <iomanip>
#include <fstream>
#include "linkerTable.hpp"
#include "myException.hpp"

void LinkerTable::makeLinkerTable(){
  addSections();
  mergeSections();

  addSymbols();
  //check symbols

  addRelocations();
}

void LinkerTable::addSections(){
  for(DataTable* dt : data_tables){
    for(Section* old_section : dt->getSections()){
      Section* new_section = findSection(old_section->getName());
      if(new_section == nullptr){
        new_section = new Section(old_section->getName());
        linker_table->addSection(new_section);
        new_section->setAddress(0);
      }
      
      //new address
      old_section->setAddress(new_section->getSize());
      //new size
      new_section->setSize(new_section->getSize() + old_section->getSize());
      //add data
      new_section->setData(old_section->getData());
    }
  }
}

void LinkerTable::addSymbols(){
  for(DataTable* dt : data_tables){
    for(Symbol* old_symbol : dt->getSymbols()){
      Symbol* symbol = findSymbol(old_symbol->getName());
      if(old_symbol->getBind() == "GLO"){
        if(symbol == nullptr){
          Section* section = findSection(old_symbol->getSection());
          Symbol* new_symbol = new Symbol(old_symbol->getName());
          int new_value = old_symbol->getValue() + section->getAddress();
          new_symbol->setValue(new_value);
          new_symbol->setBind(old_symbol->getBind());
          new_symbol->setSection(old_symbol->getSection());

          linker_table->addSymbol(new_symbol);
        }
        else if(symbol->getBind() == "GLO")
          throw MyException("Symbol '" + symbol->getName() + "' has been defined more than once.");
        else {
          Section* section = dt->findSection(old_symbol->getSection());
          int new_value = old_symbol->getValue() + section->getAddress();
          symbol->setValue(new_value);
          symbol->setBind(old_symbol->getBind());
          symbol->setSection(old_symbol->getSection());
        }
      }
      else if(old_symbol->getBind() == "EXT"){
        if(symbol == nullptr){
          Symbol* new_symbol = new Symbol(old_symbol->getName());
          new_symbol->setBind(old_symbol->getBind());
          new_symbol->setSection(old_symbol->getSection());

          linker_table->addSymbol(new_symbol);
        }
      }
      else if(old_symbol->getBind() == "SECT"){
        if(findSymbol(old_symbol->getName()))
          continue;
        Section* section = findSection(old_symbol->getSection());
        Symbol* new_symbol = new Symbol(old_symbol->getName());
        new_symbol->setBind(old_symbol->getBind());
        new_symbol->setSection(old_symbol->getSection());
        new_symbol->setValue(section->getAddress());
        linker_table->addSymbol(new_symbol);
      }
    }
  }
}

void LinkerTable::addRelocations(){
  for(DataTable* dt : data_tables){
    for(Relocation* old_relocation : dt->getRelocations()){
      Section* old_section = dt->findSection(old_relocation->getSection());
      int new_offset = old_relocation->getOffset() + old_section->getAddress();
      relocation_type new_type = old_relocation->getType();
      string new_section = old_relocation->getSection();
      string new_symbol = old_relocation->getSymbol();
      int new_addend = old_relocation->getAddend();
      /*if(dt->findSymbol(new_section)->getBind() == "SECT")
        new_addend += old_section->getAddress() - findSection(new_section)->getAddress();*/

      Relocation* new_relocation = new Relocation(new_offset, new_type, new_section, new_symbol, new_addend);

      linker_table->addRelocation(new_relocation);

      //resolve relocation
      Section* section = findSection(new_relocation->getSection());
      Symbol* symbol = findSymbol(new_relocation->getSymbol());
      if(new_type == relocation_type::R_X86_64_16){
        int val = symbol->getValue() + new_addend;
        combined_data[new_offset] = (val >> 8) & 0xff;
        combined_data[new_offset + 1] = (val & 0xff);
      }
      else if(new_type == relocation_type::R_X86_64_PC16){
        int val = symbol->getValue() + new_addend - new_offset;
        combined_data[new_offset] = (val >> 8) & 0xff;
        combined_data[new_offset + 1] = (val & 0xff);
      }
      else if(new_type == relocation_type::L_ENDIAN){
        int val = symbol->getValue() + new_addend;
        combined_data[new_offset] = (val & 0xff);
        combined_data[new_offset + 1] = (val >> 8) & 0xff;
      }
    }
  }

  /*for(Section* section : linker_table->getSections()){
    for(unsigned char byte : section->getData()){
      cout << (int)byte << " ";
    }
    cout << section->getName() << endl;
  }*/
}

void LinkerTable::mergeSections(){
  int size = 0;
  for(Section* section : linker_table->getSections()){
    section->setAddress(size);
    for(DataTable* dt : data_tables){
      for(Section* s : dt->getSections()){
        if(s->getName() == section->getName())
          s->setAddress(s->getAddress() + size);
      }
    }
    size += section->getSize();
  }

  for (Section* section: linker_table->getSections()) {
    for(unsigned char byte : section->getData())
      combined_data.push_back(byte);
  }
}

Section* LinkerTable::findSection(string name){
  for(Section* s : linker_table->getSections()){
    if(s->getName() == name)
      return s;
  }

  return nullptr;
}

Symbol* LinkerTable::findSymbol(string name){
  for(Symbol* s : linker_table->getSymbols()){
    if(s->getName() == name)
      return s;
  }

  return nullptr;
}

int LinkerTable::getTableSize(){
  int size = 0;
  for(Section* s : linker_table->getSections())
    size += s->getSize();

  return size;
}

void LinkerTable::checkSymbols(){
  for(Symbol* symbol : linker_table->getSymbols()){
    if(symbol->getBind() == "EXT")
      throw MyException("Symbol " + symbol->getName() + " is not defined.");
  }
}

void LinkerTable::writeToOutputFile(string output_file){
  fstream outputFile;
  outputFile.open(output_file, ios::out);

  if (!outputFile.is_open())
	{
		throw MyException("Unable to open output file.");
	}
  
  int val;
  int size = combined_data.size();
  int pos = 0;
  while (pos < size) {
    outputFile << hex << uppercase << setfill('0') << setw(4) << (pos & 0xFFFF) << ": ";
    for (int i = 0; i < 8; i++) {
        int j = i + pos;
        if (j < 0 || j > size) {
            outputFile << setw(2)  << 0 << " ";
        } else {
            val = combined_data[j];
            outputFile << setw(2) << val << " ";
        }
    }
    pos += 8;
    outputFile << "\n";
  }

  outputFile.close();
}

void LinkerTable::printLinkerTable(){
  fstream outputFile;
  outputFile.open("program.txt", ios::out);

  if (!outputFile.is_open())
	{
		throw MyException("Unable to open output file.");
	}

  outputFile << "SECTION HEADER TABLE" << endl;
  outputFile << left;
  outputFile << setw(20) << "Index";
  outputFile << setw(20) << "Address";
  outputFile << setw(20) << "Size";
  outputFile << setw(20) << "Name" << endl;
  int i = 0;
  for(Section* s : linker_table->getSections()){
    outputFile << setw(20) << i++; 
    outputFile << setw(20) << hex << s->getAddress();
    outputFile << setw(20) << dec << s->getSize();
    outputFile << setw(20) << s->getName() << endl;
  }

  outputFile << endl;

  outputFile << "SYMBOL TABLE" << endl;
  outputFile << left;
  outputFile << setw(20) << "Index";
  outputFile << setw(20) << "Value";
  outputFile << setw(20) << "Bind";
  outputFile << setw(20) << "Section";
  outputFile << setw(20) << "Name" << endl;
  i = 0;
  for(Symbol* s : linker_table->getSymbols()){
    outputFile << setw(20) << i++; 
    outputFile << setw(20) << hex << s->getValue();
    outputFile << setw(20) << dec << s->getBind();
    outputFile << setw(20) << dec << s->getSection();
    outputFile << setw(20) << s->getName() << endl;
  }

  outputFile << endl;

  outputFile << "RELOCATION TABLE" << endl;
  outputFile << left;
  outputFile << setw(20) << "Index";
  outputFile << setw(20) << "Offset";
  outputFile << setw(20) << "Type";
  outputFile << setw(20) << "Symbol";
  outputFile << setw(20) << "Addend" << endl;
  i = 0;
  for(Relocation* r : linker_table->getRelocations()){
    outputFile << setw(20) << i++; 
    outputFile << setw(20) << hex << r->getOffset();
    outputFile << setw(20) << dec << r->getType();
    outputFile << setw(20) << dec << r->getSymbol();
    outputFile << setw(20) << r->getAddend() << endl;
  }
}