#include <iostream>
#include <vector>
#include "myException.hpp"
#include "reader.hpp"
#include "linkerTable.hpp"

using namespace std;

int main(int argc, char *argv[]){

  try{
    //string option = argv[1];
    //string output_file = argv[3];

    vector<string> input_files;
    string output_file;

    for(int i = 1; i < argc; i++){
      if(string(argv[i]) == "-hex")
        continue;

      if(string(argv[i]) == "-o"){
        output_file = argv[++i];
        continue;
      }
      
      input_files.push_back(argv[i]);
    }

    /*for(int i = 0; i < input_files.size(); i++)
      cout << input_files[i] << endl;*/

    Reader r;
    vector<DataTable*> data_tables;
    for(string file: input_files) {
      data_tables.push_back(r.readInputFile(file));
    }

    LinkerTable lt(data_tables);
    lt.makeLinkerTable();

    lt.printLinkerTable();

    lt.writeToOutputFile(output_file);
  }
  catch(MyException e){
    cout<<e.getMessage()<<endl;
  }
  catch(...){
    cout<<"Unknown error occurred."<<endl;
  }

  return 0;
}