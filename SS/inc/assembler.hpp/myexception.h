#ifndef MYEXCEPTION_h
#define MYEXCEPTION_h

#include<exception>
#include<iostream>

using namespace std;

class MyException: public exception{
  private:
    string message;
  
  public:
    MyException(string message){
      this->message = message;
    }

    string getMessage(){
      return message;
    }
};

#endif