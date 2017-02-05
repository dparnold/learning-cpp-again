#include "MyClass.h"
#include <iostream>
using namespace std;
//:: == resolution operator which allows to define the class functions declared in .h
MyClass::MyClass(int a, int b) //constructor
: regVar(a), constVar(b) //NO semicolon here
// constructor initializer list
//Constant values must be initialized here
//Same as regVar = a;   constVar = b;
{
    cout<<"Constructor used"<<endl;
    cout << regVar << endl;
    cout << constVar << endl;
}



MyClass::~MyClass()
{
    cout<<"Destructor used"<<endl;
}

void MyClass::myPrint()
{
    cout<<"Hello"<<endl;
}

void MyClass::myPrint() const
{
    cout<<"myPrint used for constant instance"<<endl;
}

void MyClass::printFloat(float value)
{
    cout<<value<<endl;
}
