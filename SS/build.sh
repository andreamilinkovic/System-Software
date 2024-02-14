g++ -o assembler -I ./inc/assembler.hpp  ./src/assembler.cpp/*.cpp
g++ -o linker -I ./inc/linker.hpp  ./src/linker.cpp/*.cpp
g++ -o emulator -I ./inc/emulator.hpp  ./src/emulator.cpp/*.cpp