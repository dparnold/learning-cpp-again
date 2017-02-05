
#include <iostream>
#include "MyClass.h"
using namespace std;

const float pi = 3.14; // Must be initialized when created
int main() {
    MyClass obj(2,3); //Create a MyClass object
    obj.myPrint();
    obj.printFloat(pi);
    MyClass *ptr = &obj; // Do the same using a pointer for obj
    ptr->myPrint(); // Arrow member selection operator (->) is used to access an object's members with a pointer
    // working with: objects. | pointers->

    const MyClass objConst(3,4);
    objConst.myPrint();     //This will call void myPrint() const;
    return 0;
}
