#ifndef MYCLASS_H // if not defined
#define MYCLASS_H // -> define it
// needed if several files in one project include the same headers


class MyClass
{
    public:
        MyClass(int a, int b);      //Constructor
        ~MyClass();     //Destructor not mandatory only if you need one

        void myPrint();
        void myPrint() const;   //The same function can be created for constant instances

        void printFloat(float value);
    protected:
    private:
        int regVar;
        const int constVar;
        //MyClass2 obj2; //Here can also be another object
        //This is called a Composition. Like a car beeing made of a lot of separate parts.

};

#endif // MYCLASS_H
