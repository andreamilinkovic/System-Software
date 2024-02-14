#ifndef SECTION_HPP
#define SECTION_HPP

#include <iostream>
#include <vector>
#include<iomanip>

using namespace std;

class Section{
  private:
    int address;
    int size;
    string name;
    vector<unsigned char> data;

  public:
    Section(string name_, int size_, int address_): name(name_), size(size_), address(address_){}
    Section(string name_) : name(name_), address(0), size(0){}
    
    string getName(){ return name; }
    int getSize(){ return size; }
    int getAddress(){ return address; }
    vector<unsigned char>& getData(){ return data; }

    void setSize(int size_){ size = size_; }
    void setAddress(int address_){ address = address_; }
    void setData(vector<unsigned char> data_){
      for(unsigned char byte : data_)
        data.push_back(byte);
    }
};

#endif