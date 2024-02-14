#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include "emulator.hpp"
#include "myException.hpp"
#include "instruction.hpp"

Emulator::Emulator(){
  ram = new unsigned char[MEMORY_SIZE];
  if(ram == nullptr)
    throw MyException("RAM cannot be allocated.");

  for (int i = 0; i < MEMORY_SIZE; i++) {
     ram[i] = 0;
  }

  for (int i = 0; i < 5; i++) {
     instruction_reg[i] = 0;
  }
}

void Emulator::readInputFile(string input_file){
  fstream inputFile;
  inputFile.open(input_file, ios::in);
  if(!inputFile.is_open())
    throw MyException("Input file cannot be opened.");

  string line;
  while(getline(inputFile, line)){
    string address_str;
    stringstream line_str(line);
    line_str >> address_str;
    address_str = address_str.substr(0, address_str.length() - 1);
    int address = stoi(address_str, nullptr, 16) & 0xffff;

    //insert to ram
    for(int i = 0; i < 8; i++){
      string byte_str;
      line_str >> byte_str;
      int byte = stoi(byte_str, nullptr, 16);
      ram[address + i] = (uint8_t)byte;
    }
  }

  inputFile.close();

  //setRegister(6, 0xe000);
  setRegister(7, 0x005c);

  /*fstream outputFile;
  outputFile.open("test.txt", ios::out);
  for(int i = 0; i < MEMORY_SIZE; i++){
    if(i % 8 == 0 && i != 0) outputFile << endl;
    outputFile << hex << setw(2) << setfill('0') << (int)ram[i] << " "; 
  }*/
}

void Emulator::emulate(){
  bool end = false;
  while(!end){
    instruction_reg[0] = readPCptr() & 0xff;
    uint8_t op_code = instruction_reg[0];
    switch(op_code){
      case HALT:{
        //cout << "halt" << endl;
        end = true;
        break;
      }
      case INT:{
        //cout << "int" << endl;
        instruction_reg[1] = readPCptr();
        processINT();
        break;
      }
      case IRET:{
        //cout << "iret" << endl;
        processIRET();
        break;
      }
      case RET:{
        //cout << "ret" << endl;
        processRET();
        break;
      }
      case CALL:
      case JMP:
      case JEQ:
      case JNE:
      case JGT:{
        //cout << "jump instruction" << endl;
        instruction_reg[1] = readPCptr();
        instruction_reg[2] = readPCptr();
        processJMP_INSTR();
        break;
      }
      case XCHG:{
        //cout << "xchg" << endl;
        instruction_reg[1] = readPCptr();
        processXCHG();
        break;
      }
      case ADD:
      case SUB:
      case MUL:
      case DIV:
      case CMP:{
        //cout << "aritm instruction" << endl;
        instruction_reg[1] = readPCptr();
        processARITM_INSTR();
        break;
      }
      case NOT:
      case AND:
      case OR:
      case XOR:
      case TEST:{
        //cout << "logic instruction" << endl;
        instruction_reg[1] = readPCptr();
        processLOG_INSTR();
        break;
      }
      case SHL:{
        //cout << "shl" << endl;
        instruction_reg[1] = readPCptr();
        processSHL();
        break;
      }
      case SHR:{
        //cout << "shr" << endl;
        instruction_reg[1] = readPCptr();
        processSHR();
        break;
      }
      case LDR:
      case STR:{
        //cout << "load/store" << endl;
        instruction_reg[1] = readPCptr();
        instruction_reg[2] = readPCptr();
        processLDR_STR();
        break;
      }
      default:{
        processINVALID();
        break;
      }
    }
  }

  writeRegisters();
}

uint16_t Emulator::readPCptr(){
  uint16_t address = getRegister(7);
  uint16_t ret = ram[getRegister(7)];
  incPC();
  return ret;
}

void Emulator::incPC(){
  uint16_t PC = getRegister(7);
  setRegister(7, PC + 1);
}

uint16_t Emulator::getRegister(int reg){
  return registers[reg];
}

void Emulator::setRegister(int reg, uint16_t val){
  registers[reg] = val;
}

void Emulator::push(uint16_t data){
  uint16_t SP = getRegister(6);
  SP -= 2;
  writeLittleEndian(SP, data);
  setRegister(6, SP);
}

uint16_t Emulator::pop(){
  uint16_t SP = getRegister(6);
  uint16_t data = readLittleEndian(SP);
  setRegister(6, SP + 2);

  return data;
}

void Emulator::loadInstruction(){
  int type = instruction_reg[2] & 0xf;
  
  switch(type){
    case IMMEDIATE:
    case REGISTER_INDIRECT_WITH_SIGNED_OFFSET:
    case MEMORY_DIRECT:
    case REGISTER_DIRECT_WITH_SIGNED_OFFSET:{
      instruction_reg[3] = readPCptr();
      instruction_reg[4] = readPCptr();
      break;
    }
  }
}

uint16_t Emulator::readLittleEndian(uint16_t address){
  uint8_t low = ram[address];
  uint8_t high = ram[address + 1];

  return (high << 8) | low;
}

void Emulator::writeLittleEndian(uint16_t address, uint16_t data){
  uint8_t high = ((data >> 8) & 0xff);
  uint8_t low = data & 0xff;
  ram[address] = low;
  ram[address + 1] = high;
}

int Emulator::getN(){
  return (getRegister(8) >> 3) & 0x1;
}
void Emulator::setN(uint16_t val){
  if(val == 0)
    setRegister(8, getRegister(8) & 0xfff7);
  else
    setRegister(8, getRegister(8) | 0x0008);
}
int Emulator::getC(){
  return (getRegister(8) >> 2) & 0x1; 
}
void Emulator::setC(uint16_t val){
  if(val == 0)
    setRegister(8, getRegister(8) & 0xfffb);
  else
    setRegister(8, getRegister(8) | 0x0004);
}
int Emulator::getO(){
  return (getRegister(8) >> 1) & 0x1;
}
void Emulator::setO(uint16_t val){
  if(val == 0)
    setRegister(8, getRegister(8) & 0xfffd);
  else
    setRegister(8, getRegister(8) | 0x0002);
}
int Emulator::getZ(){
  return getRegister(8) & 0x1; 
}
void Emulator::setZ(uint16_t val){
  if(val == 0)
    setRegister(8, getRegister(8) & 0xfffe);
  else
    setRegister(8, getRegister(8) | 0x0001);
}

void Emulator::processINT(){
  push(getRegister(7));
  push(getRegister(8));
  int regD = (instruction_reg[1] >> 4) & 0xf;
  int entry = (getRegister(regD) % 8) * 2;
  int data = readLittleEndian(entry);
  setRegister(7, data);
}

void Emulator::processIRET(){
  setRegister(8, pop());
  setRegister(7, pop());
}

void Emulator::processRET(){
  setRegister(7, pop());
}

void Emulator::processJMP_INSTR(){
  //TODO
  loadInstruction();
  bool jump = false;
  switch(instruction_reg[0]){
    case CALL:{
      jump = true;
      push(getRegister(7));
      break;
    }
    case JMP:{
      jump = true;
      break;
    }
    case JEQ:{
      jump = getZ();
      break;
    }
    case JNE:{
      jump = !getZ();
      break;
    }
    case JGT:{
      jump = (!(getN() ^ getO())) && (!getZ());
      break;
    }
  }

  if(jump){
    int regD = (instruction_reg[1] >> 4) & 0xf;
    int regS = instruction_reg[1] & 0xf;
    int dest = getRegister(regD);
    int source = getRegister(regS);

    int mode = (instruction_reg[2] >> 4) & 0xf;
    if(mode == DEC_BY_2_BEFORE_FORMING_ADDRESS){
      source -= 2;
      setRegister(regS, source);
    }
    if(mode == INC_BY_2_BEFORE_FORMING_ADDRESS){
      source += 2;
      setRegister(regS, source);
    }

    int type = instruction_reg[2] & 0xf;
  
    switch(type){
      case IMMEDIATE:{
        uint16_t data = (instruction_reg[3] << 8) | instruction_reg[4];
        setRegister(7, data);
        break;
      }
      case REGISTER_DIRECT:{
        setRegister(7, source);
        break;
      }
      case REGISTER_INDIRECT:{
        uint16_t data = readLittleEndian(source);
        setRegister(7, data);
        break;
      }
      case REGISTER_INDIRECT_WITH_SIGNED_OFFSET:{
        uint16_t offset = (instruction_reg[3] << 8) | instruction_reg[4];
        uint16_t data = readLittleEndian(source + offset);
        setRegister(7, data);
        break;
      }
      case MEMORY_DIRECT:{
        uint16_t address = (instruction_reg[3] << 8) | instruction_reg[4];
        uint16_t data = readLittleEndian(address);
        setRegister(7, data);
        break;
      }
      case REGISTER_DIRECT_WITH_SIGNED_OFFSET:{
        uint16_t offset = (instruction_reg[3] << 8) | instruction_reg[4];
        uint16_t data = source + offset;
        setRegister(7, data);
        break;
      }
    }

    if(mode == DEC_BY_2_AFTER_FORMING_ADDRESS){
    setRegister(regS, source - 2);
    }
    if(mode == INC_BY_2_AFTER_FORMING_ADDRESS){
      setRegister(regS, source + 2);
    }
  }
  else{
    return;
  }    
}

void Emulator::processXCHG(){
  int regD = (instruction_reg[1] >> 4) & 0xf;
  int regS = instruction_reg[1] & 0xf;
  int a = getRegister(regD);
  int b = getRegister(regS);
  setRegister(regD, b);
  setRegister(regS, a);
}

void Emulator::processARITM_INSTR(){
  int regD = (instruction_reg[1] >> 4) & 0xf;
  int regS = instruction_reg[1] & 0xf;
  int a = getRegister(regD);
  int b = getRegister(regS);
  switch(instruction_reg[0]){
    case ADD:{
      setRegister(regD, a + b);
      break;
    }
    case SUB:{
      setRegister(regD, a - b);
      break;
    }
    case MUL:{
      setRegister(regD, a * b);
      break;
    }
    case DIV:{
      setRegister(regD, a / b);
      break;
    }
    case CMP:{
      setZ(a == b);
      setN((a - b) < 0);
      setC(a < b);
      setO((a > 0 && b < 0 && (a - b < 0)) || (a < 0 && b > 0 && (a - b > 0)));
      break;
    }
  }
}

void Emulator::processLOG_INSTR(){
  int regD = (instruction_reg[1] >> 4) & 0xf;
  int regS = instruction_reg[1] & 0xf;
  int a = getRegister(regD);
  int b = getRegister(regS);
  switch(instruction_reg[0]){
    case NOT:{
      setRegister(regD, ~a);
      break;
    }
    case AND:{
      setRegister(regD, a & b);
      break;
    }
    case OR:{
      setRegister(regD, a | b);
      break;
    }
    case XOR:{
      setRegister(regD, a ^ b);
      break;
    }
    case TEST:{
      setZ((a & b) == 0);
      setN((a & b) < 0);
      break;
    }
  }
}

void Emulator::processSHL(){
  int regD = (instruction_reg[1] >> 4) & 0xf;
  int regS = instruction_reg[1] & 0xf;
  int a = getRegister(regD);
  int b = getRegister(regS);

  setRegister(regD, a << b);

  setC(((a >> 15) & 0x1) != 0);
  setN((a << b) < 0);
  setZ((a << b) == 0); 
}

void Emulator::processSHR(){
  int regD = (instruction_reg[1] >> 4) & 0xf;
  int regS = instruction_reg[1] & 0xf;
  int a = getRegister(regD);
  int b = getRegister(regS);

  setRegister(regD, a >> b);

  setC((a & 0x1) != 0);
  setN((a >> b) < 0);
  setZ((a >> b) == 0); 
}

void Emulator::processLDR_STR(){
  loadInstruction();
  int regD = (instruction_reg[1] >> 4) & 0xf;
  int regS = instruction_reg[1] & 0xf;
  int dest = getRegister(regD);
  int source = getRegister(regS);

  int mode = (instruction_reg[2] >> 4) & 0xf;
  if(mode == DEC_BY_2_BEFORE_FORMING_ADDRESS){
    source -= 2;
    setRegister(regS, source);
  }
  if(mode == INC_BY_2_BEFORE_FORMING_ADDRESS){
    source += 2;
    setRegister(regS, source);
  }

  if(instruction_reg[0] == LDR){
    int type = instruction_reg[2] & 0xf;
  
    switch(type){
      case IMMEDIATE:{
        uint16_t data = (instruction_reg[3] << 8) | instruction_reg[4];
        setRegister(regD, data);
        break;
      }
      case REGISTER_DIRECT:{
        setRegister(regD, source);
        break;
      }
      case REGISTER_INDIRECT:{
        uint16_t data = readLittleEndian(source);
        setRegister(regD, data);
        break;
      }
      case REGISTER_INDIRECT_WITH_SIGNED_OFFSET:{
        uint16_t offset = (instruction_reg[3] << 8) | instruction_reg[4];
        uint16_t data = readLittleEndian(source + offset);
        setRegister(regD, data);
        break;
      }
      case MEMORY_DIRECT:{
        uint16_t address = (instruction_reg[3] << 8) | instruction_reg[4];
        uint16_t data = readLittleEndian(address);
        setRegister(regD, data);
        break;
      }
      case REGISTER_DIRECT_WITH_SIGNED_OFFSET:{
        uint16_t offset = (instruction_reg[3] << 8) | instruction_reg[4];
        setRegister(regD, source + offset);
        break;
      }
    }
  }
  else{
    int type = instruction_reg[2] & 0xf;
  
    switch(type){
      case IMMEDIATE:{
        processINVALID();
        break;
      }
      case REGISTER_DIRECT:{
        setRegister(regS, dest);
        break;
      }
      case REGISTER_INDIRECT:{
        writeLittleEndian(source, dest);
        break;
      }
      case REGISTER_INDIRECT_WITH_SIGNED_OFFSET:{
        uint16_t offset = (instruction_reg[3] << 8) | instruction_reg[4];
        uint16_t address = source + offset;
        writeLittleEndian(address, dest);
        break;
      }
      case MEMORY_DIRECT:{
        uint16_t address = (instruction_reg[3] << 8) | instruction_reg[4];
        writeLittleEndian(address, dest);
        break;
      }
      case REGISTER_DIRECT_WITH_SIGNED_OFFSET:{
        processINVALID();
        break;
      }
    }
  }

  if(mode == DEC_BY_2_AFTER_FORMING_ADDRESS){
    setRegister(regS, source - 2);
  }
  if(mode == INC_BY_2_AFTER_FORMING_ADDRESS){
    setRegister(regS, source + 2);
  }
}

void Emulator::processINVALID(){
  processIVTentry(1);
}

void Emulator::processIVTentry(int entry){
  uint16_t address = readLittleEndian(entry * 2);

  push(7);
  push(8);
  setRegister(7, address);
}

void Emulator::writeRegisters(){
  cout << "Emulated processor executed halt instruction" << endl;
  cout << "Emulated processor state: psw=0b" << bitset<16>(getRegister(8)) << endl;
  cout << dec << "r0=0x" << hex << getRegister(0) << " ";
  cout << dec << "r1=0x" << hex << getRegister(1) << " ";
  cout << dec << "r2=0x" << hex << getRegister(2) << " ";
  cout << dec << "r3=0x" << hex << getRegister(3) << endl; 
  cout << dec << "r4=0x" << hex << getRegister(4) << " ";
  cout << dec << "r5=0x" << hex << getRegister(5) << " ";
  cout << dec << "r6=0x" << hex << getRegister(6) << " ";
  cout << dec << "r7=0x" << hex << getRegister(7) << endl; 
}