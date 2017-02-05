#include <iostream>
#include <math.h>  // floor(x)
#include <iomanip>      // std::setprecision
#include <cstdlib> // for rand()
#include <ctime> // for time()
#include <string> // for use in string
#include <fstream> // for file output
using namespace std;

float round(float value, int digits)
{
	value *= int(pow(10,digits));
	value += 0.5;
	value = floor(value);
	value/= pow(10,digits);
	return value;
}
double round(double value, int digits) //  Declaring the same function with different return types AND arguments or only arguments is called "function overloading"
{
	value *= int(pow(10,digits));
	value += 0.5;
	value = floor(value);
	value/= pow(10,digits);
	return value;
}

int factorial(int i)
{
	if(i==1)
	{
		return 1;
	}
	else
	{
		return i*factorial(i-1);
	}
}

void PrintRandomNumbers( int max_value, int n=3) // It is not possible to set a default value only for the first argument
{
	int seed = time(0);
	cout << "seed = " << seed << " s\n";
	srand(seed); // create set current second count as seed
	int i;
	for(i=0;i<n;i++)
	{
		cout << rand()%max_value << endl;
	}
}


// A bit of OOP
class FancyCar
{
public:
    FancyCar(int money) { // constructor for a new object. NO return type. SAME NAME as class.
      cout <<"You just bought a new car!\n";
      money_in_glove_compartment = money;
    }

	void look_at()
	{
		cout<<"What a nice car!\n";
	}
	void change_money_in_glove_compartment(int amount, int code_input) // principle of encapsulation
	{
		if(code_input == code)
		{
			money_in_glove_compartment+= amount;
			cout << money_in_glove_compartment << "\n";
		}
		else
		{
			cout << "Error wrong code!\n";
		}

	}
private:
	int money_in_glove_compartment; // Can only be accesed from within the class
	int code = 1234;
}; // ; has to be there!


int main()
{
	int *int_pointer; // pointer to int
	int i = 20;
	int_pointer = &i; // set pointer to int i
	cout << i << "\t"<< int_pointer<< endl;

	cout << factorial(12) << endl;
	PrintRandomNumbers(1000,2);
	PrintRandomNumbers(10); // use default value

	FancyCar aston(3000); // create an instance(=object) of FancyCar called aston and put 3000¤ in the glove compartment
	aston.look_at(); // call a method of the aston
	aston.change_money_in_glove_compartment(-100,1245);
	aston.change_money_in_glove_compartment(-100,1234);

	string fileName = "file.txt";
	ofstream outputFile;

	outputFile.open(fileName.c_str()); // constructor taces char* not string bevore c++11
	outputFile << "This is a test file!\n";
	outputFile << "I can even put numbers in there." << 23 << endl;
	outputFile.close(); // put everything in there
}
