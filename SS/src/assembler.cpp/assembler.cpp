#include<fstream>

#include "assembler.h"
#include "myexception.h"
#include "instruction.h"
#include "token.h"
#include "sectionHeaderTable.h"

const string delimiters = ", \f\n\r\t\v";

void Assembler::parseInputFile(string input_file){
  fstream inputFile;
  inputFile.open(input_file, ios::in);
  if(!inputFile.is_open())
    throw MyException("Input file cannot be opened.");

  string line;
  while(getline(inputFile, line)){
    if(line.empty())
      continue;

    //parse line
    queue<string> parsedLine;

    size_t start = line.find_first_not_of(delimiters, 0);
    size_t end = line.find_first_of(delimiters, start);

    while(start != string::npos || end != string::npos){
      string token = line.substr(start, end - start);
      size_t comment = token.find('#', 0);
      if(comment != string::npos){
        token = token.substr(0, comment);
        if(!token.empty()){
          parsedLine.push(token);
        }
        break;
      }
      parsedLine.push(token);
      start = line.find_first_not_of(delimiters, end);
      end = line.find_first_of(delimiters, start);
    }

    if(!parsedLine.empty()){
      parsedInputFile.push_back(parsedLine);
      if(parsedLine.front() == ".end")
        break;
    }
  }

  if(parsedInputFile.empty())
    throw MyException("Input file is empty.");

  if(parsedInputFile.back().back() != ".end")
    throw MyException("File must end with an .end directive.");

  inputFile.close();

  //test
  /*for (queue<string> line : parsedInputFile) {
		int n = line.size();
		for (int i = 0; i < n;++i) {
			cout << line.front() + " ";
			line.pop();
		}
		cout << endl;
  }*/
}

void Assembler::firstPass(){
  //section_type curr_section = section_type::NO_SECTION;
  string curr_section_name = "";
	uint16_t lc = 0; //location counter
  vector<sym_table_entry>& sym_table = SectionHeaderTable::getSymbolTable()->sym_table;

  queue<string> line_of_tokens;
  unsigned long number_of_lines_in_input_file = parsedInputFile.size();

	for (unsigned long lineNo = 0; lineNo < number_of_lines_in_input_file;)
	{
    if(line_of_tokens.empty())
	    line_of_tokens = parsedInputFile[lineNo];
    Token curr_token = Token::determineTokenType(line_of_tokens.front()); //mnemonic
	  line_of_tokens.pop();

    switch(curr_token.getType()){
      case TokenType::ACCESS_RIGHTS_DIRECTIVE:{ //.global, .extern
        while(!line_of_tokens.empty()){
          string symbol = line_of_tokens.front();
          line_of_tokens.pop();

          unsigned long i = SectionHeaderTable::getSymbolTable()->findSymbol(symbol);
          if(i == 0){
            //symbol not in sym_table -> insert new symbol
            if(curr_token.getValue() == ".global"){
              SectionHeaderTable::getSymbolTable()->insertSymbol(symbol, 0, 0, bind_type::GLOBAL, false);
            }
            else if(curr_token.getValue() == ".extern"){
              SectionHeaderTable::getSymbolTable()->insertSymbol(symbol, 0, 0, bind_type::EXTERN, false);
            }
          }
          else{
            //symbol already exists in sym_table
            throw MyException("Symbol '" + symbol + "' already exists in symbol table. Error at line: " + to_string(lineNo + 1));
          }
        }
        break;
      }
      case TokenType::MEMORY_ALLOCATION_DIRECTIVE:{ //.skip, .word
        if(curr_token.getValue() == ".skip"){
          string operand = line_of_tokens.front();
          line_of_tokens.pop();

          uint16_t n = stoi(operand, nullptr, 0);
          for(uint16_t i = 0; i < n; i++){
            SectionHeaderTable::getSectionTable(curr_section_name)->insertByte(0x00);
          }
          lc += n;
        }
        else if(curr_token.getValue() == ".word"){
          while(!line_of_tokens.empty()){
            Token operand = Token::determineTokenType(line_of_tokens.front());
            line_of_tokens.pop();

            if(operand.getType() != TokenType::SYMBOL_OPERAND){
              int16_t value = stoi(operand.getValue(), nullptr, 0);
              SectionHeaderTable::getSectionTable(curr_section_name)->insertWord(value);
            }
            else{ //operand is symbol
              SectionHeaderTable::getSectionTable(curr_section_name)->insertWord(0x0000);

              unsigned long i = SectionHeaderTable::getSymbolTable()->findSymbol(operand.getValue());
              if(i == 0){
                //symbol not in sym_table -> insert new symbol, make flink entry
                SectionHeaderTable::getSymbolTable()->insertSymbol(operand.getValue(), 0, 0, bind_type::LOCAL, false);
                uint16_t index = SectionHeaderTable::getSymbolTable()->getSym_tableSize() - 1;
                flink flink_ = flink(lc, curr_section_name, relocation_type::L_ENDIAN);
                sym_table[index].fwdref.push_back(flink_);
              }
              else if(i != 0 && sym_table[i].defined == false){
                //symbol in sym_table but not defined -> make flink entry
                flink flink_ = flink(lc, curr_section_name, relocation_type::L_ENDIAN);
                sym_table[i].fwdref.push_back(flink_);
              } else if(i != 0 && sym_table[i].defined == true){
                //symbol in sym_table and is defined -> make relocation entry
                if(sym_table[i].bind == bind_type::EXTERN || sym_table[i].bind == bind_type::GLOBAL){
                  SectionHeaderTable::getRelocationTable(curr_section_name)->
                    createEntry(lc, relocation_type::L_ENDIAN, i, 0);
                }
                else{
                  SectionHeaderTable::getRelocationTable(curr_section_name)->
                    createEntry(lc, relocation_type::L_ENDIAN, sym_table[i].section, sym_table[i].value);
                }
              }
            }
            lc += 2;
          }
        }
        break;
      }
      case TokenType::SECTION_DIRECTIVE:{
        //update size of previous section in sym_table
        SectionHeaderTable::getSymbolTable()->updateSymbolSize(lc, "." + curr_section_name);
        
        curr_section_name = line_of_tokens.front();
        line_of_tokens.pop();
        lc = 0;

        unsigned long i = SectionHeaderTable::getSymbolTable()->findSymbol("." + curr_section_name);
        if(i == 0){
          //symbol not in sym_table -> insert new symbol
          SectionHeaderTable::getSymbolTable()->insertSymbol("." + curr_section_name, 0, 0, bind_type::LOCAL, true);
          uint16_t index = SectionHeaderTable::getSymbolTable()->getSym_tableSize() - 1;
          sym_table[index].section = index;
          sym_table[index].type = sym_type::SECT;
        }
        else{
          throw MyException("Section '." + curr_section_name + "' already exists. Error at line: " + to_string(lineNo + 1));
        }
        break;
      }
      case TokenType::END_DIRECTIVE:{
        SectionHeaderTable::getSymbolTable()->updateSymbolSize(lc, "." + curr_section_name);
			  lc = 0;
        break;
      }
      case TokenType::LABEL:{
        string symbol = curr_token.getValue();
        unsigned long i = SectionHeaderTable::getSymbolTable()->findSymbol(symbol);
        if(i == 0){
          //symbol not in symbol table -> insert symbol
          unsigned long section = SectionHeaderTable::getSymbolTable()->findSymbol("." + curr_section_name); 
          SectionHeaderTable::getSymbolTable()->insertSymbol(symbol, lc, section, bind_type::LOCAL, true);
        }
        else{
          //symbol exists in symbol table -> update
          if(sym_table[i].defined == true)
            throw MyException("Symbol '" + symbol + "' is alredy defined. Error at line: " + to_string(lineNo + 1));

          sym_table[i].value = lc;
          unsigned long section = SectionHeaderTable::getSymbolTable()->findSymbol("." + curr_section_name);
          sym_table[i].section = section;
          sym_table[i].defined = true;
        }
        if(!line_of_tokens.empty()){
          continue;
        }
        break;
      }
      case TokenType::INSTRUCTION:{
        lc += Instruction::processInstruction(sym_table, curr_token.getValue(), line_of_tokens, lc, curr_section_name, lineNo);
        line_of_tokens = queue<string>();
        break;
      }
    }
    lineNo++;
  }
}

void Assembler::backpatching(){
  vector<sym_table_entry>& sym_table = SectionHeaderTable::getSymbolTable()->sym_table;

  for(sym_table_entry symbol_entry : sym_table){
    for(flink flink_ : symbol_entry.fwdref){
      if(symbol_entry.defined == true){
        switch (symbol_entry.bind){
          case bind_type::LOCAL:{

            switch(flink_.type){
              case relocation_type::R_X86_64_16:{
                SectionHeaderTable::getRelocationTable(flink_.section)
                  ->createEntry(flink_.offset, flink_.type, symbol_entry.section, flink_.addend + symbol_entry.value);
                break;
              }
              case relocation_type::R_X86_64_PC16:{
                if(SectionHeaderTable::getSymbolTable()->findSymbol(flink_.section ) == symbol_entry.section){
                  //no new relocation entry -> write to Section table
                  SectionHeaderTable::getSectionTable(flink_.section)->insertWordtoAddress(symbol_entry.value, flink_.offset);
                }
                else {
                  SectionHeaderTable::getRelocationTable(flink_.section)
                  ->createEntry(flink_.offset, flink_.type, symbol_entry.section, flink_.addend + symbol_entry.value);
                }
                break;
              }
            }

            break;
          }
          case bind_type::GLOBAL:{
            switch(flink_.type){
              case relocation_type::R_X86_64_16:{
                SectionHeaderTable::getRelocationTable(flink_.section)->createEntry(flink_.offset, flink_.type, 
                  SectionHeaderTable::getSymbolTable()->findSymbol(symbol_entry.name), flink_.addend + symbol_entry.value);
                break;
              }
              case relocation_type::R_X86_64_PC16:{
                if(SectionHeaderTable::getSymbolTable()->findSymbol(flink_.section ) == symbol_entry.section){
                  //no new relocation entry -> write to Section table
                  SectionHeaderTable::getSectionTable(flink_.section)->insertWordtoAddress(symbol_entry.value, flink_.offset);
                }
                else {
                  SectionHeaderTable::getRelocationTable(flink_.section)->createEntry(flink_.offset, flink_.type, 
                    SectionHeaderTable::getSymbolTable()->findSymbol(symbol_entry.name), flink_.addend + symbol_entry.value);
                }
                break;
              }
            }

            break;
          }
        }
      }
      else {
        if(symbol_entry.bind == bind_type::EXTERN){
          SectionHeaderTable::getRelocationTable(flink_.section)->createEntry(flink_.offset, flink_.type, 
             SectionHeaderTable::getSymbolTable()->findSymbol(symbol_entry.name), flink_.addend);
        }
        else {
          throw MyException("Symbol '" + symbol_entry.name + "' is not defined.");
        }
      }
    }
  }
}

void Assembler::writeToOutputFile(string output_file){
  fstream outputFile;
  outputFile.open(output_file, ios::out);

  if (!outputFile.is_open())
	{
		throw MyException("Unable to open output file.");
	}

  SectionHeaderTable::writeSHTableToOutputFile(outputFile);

  outputFile.close();
}