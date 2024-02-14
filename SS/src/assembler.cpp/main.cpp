#include<iostream>

#include"myexception.h"
#include "assembler.h"
#include "binaryPrinter.h"
#include "sectionHeaderTable.h"

using namespace std;

int main(int argc, char** argv){

  try{
    Assembler as = Assembler();
    BinaryPrinter bp = BinaryPrinter();

    as.parseInputFile(argv[3]);
    as.firstPass();
    as.backpatching();
    as.writeToOutputFile((string)argv[2] + ".txt");
    bp.printToOutputFileBinary(argv[2]);
  }
  catch(MyException e){
    cout<<e.getMessage()<<endl;
  }
  catch(...){
    cout<<"Unknown error occurred."<<endl;
  }

  return 0;
}