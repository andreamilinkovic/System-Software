#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <iostream>

using namespace std;

const int MEMORY_SIZE = 0xffff;

class Emulator{
  private:
    uint8_t* ram = nullptr;
    uint8_t instruction_reg[5];
    uint16_t registers[9];
  
  public:
    Emulator();

    void readInputFile(string input_file);

    void emulate();

    uint16_t readPCptr();
    void incPC();

    uint16_t getRegister(int reg);
    void setRegister(int reg, uint16_t val);

    void push(uint16_t data);
    uint16_t pop();

    void loadInstruction();

    uint16_t readLittleEndian(uint16_t data);
    void writeLittleEndian(uint16_t address, uint16_t data);

    int getN();
    void setN(uint16_t val);
    int getC();
    void setC(uint16_t val);
    int getO();
    void setO(uint16_t val);
    int getZ();
    void setZ(uint16_t val);

    void processINT();
    void processIRET();
    void processRET();
    void processJMP_INSTR();
    void processXCHG();
    void processARITM_INSTR();
    void processLOG_INSTR();
    void processSHL();
    void processSHR();
    void processLDR_STR();
    void processINVALID();

    void processIVTentry(int entry);

    void writeRegisters();
};

#endif