#include "Tutor.h"
#include <iostream>
#include <string>

using namespace std;
int main()
{
	string input;
	cout << "Enter commands (write 'exit' to quit): \n";
	
	do
	{
		cout << "> ";
		if(!getline(cin, input)) break;
		
		dsds
		
	}while(input != 'exit');
	
	
    Tutor tutor("passages.txt");
    tutor.createHeap();
    tutor.stats();
    tutor.setTime("20m");
}
