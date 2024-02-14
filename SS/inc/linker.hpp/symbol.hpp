#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <iostream>

using namespace std;

enum bind_type{
  SECT,
  GLO,
  EXT
};

class Symbol{
  private:
    int value;
    string bind;
    string section;
    string name;

  public:
    Symbol(string name_, string section_, int value_, string bind_): name(name_), section(section_), value(value_), bind(bind_){}
    Symbol(string name_): name(name_), value(0){}

    int getValue(){ return value; }
    string getBind(){ return bind; }
    string getSection(){ return section; }
    string getName(){ return name; }

    void setValue(int value_){ value = value_; }
    void setBind(string bind_){ bind = bind_; }
    void setSection(string section_){ section = section_; }
};

#endif