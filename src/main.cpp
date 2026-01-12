//author: Simone Piccinini

#include "tutor.h"
#include <iostream>
#include <string>
#include <sstream>


using namespace std;
int main()
{
	
	string input;
	cout << "Enter commands (write 'exit' to quit): \n";
	
	Tutor tutor("Passages.txt");
	
	do
	{
		cout << "> ";
		if(!getline(cin, input) || input == "exit") break;
		
		stringstream ss(input);
		string command;
		ss >> command;
		
		if(command == "set_time")
		{
			
			string timeVal;
			if(ss >> timeVal)
			{
					cout << "set_time on: " << timeVal << "\n"; 
					
					//setting time according to input
					tutor.set_time(timeVal);
			}
			else
			{
					cout << "No valid format for set_time: \n";
			}
		}
		else if(command == "reset")
		{
			tutor.reset();
			cout << "reset done... \n" ; 
		}
		else if(command == "stats")
		{
			cout << "stats view: \n"; 
			tutor.stats();
		}
		
		
	}while(true);
	
    

}
