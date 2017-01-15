#include<iostream> // Use the header file iostream which defines the cin, cout, cerr and clog objects
using namespace std; // Tells the compiler to use the standard namespace. This way you do not need to write std::cout << "output";

int main ()
    {
    int n; // Declare a new integer called n
    cout << "Enter an integer number: "; // Sending data to the output
    cin >> n; // Read a value and store it as n
    cout << "You entered the following number: " << n << endl; // Print the values in the standard output. endl creates a new line
    cout << "Another string \n";
    cout << "Backslash n should do the same as endl \n";
    }

// This programme is compiled by using: g++ 1_text_IO.cpp -o 1_text_IO

