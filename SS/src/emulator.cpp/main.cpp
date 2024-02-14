#include <iostream>
#include "myException.hpp"
#include "emulator.hpp"

using namespace std;

int main(int argc, char *argv[]){
  try{
    string input_file = argv[1];

    Emulator em;
    em.readInputFile(input_file);
    em.emulate();
  }
  catch(MyException e){
    cout<<e.getMessage()<<endl;
  }
  catch(...){
    cout<<"Unknown error occurred."<<endl;
  }

  return 0;
}