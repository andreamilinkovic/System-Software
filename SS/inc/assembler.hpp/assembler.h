#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <vector>
#include <queue>
#include <string>

using namespace std;

class Assembler{
  private:
   
  public:
    vector<queue<string>> parsedInputFile;

    void parseInputFile(string input_file);

    void firstPass();

    void backpatching();

    void writeToOutputFile(string output_file);
};

#endif