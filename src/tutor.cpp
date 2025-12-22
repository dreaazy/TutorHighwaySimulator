//Author: Simone Piccinini

//used a min-heap for managing the events
//this allows to have O(log n) in extracting the minor time

#include "../include/tutor.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

// I want a min heap, because I want to extract before the smallest event
// priority queue uses < to compare elements, according to time
bool Event::operator<(const Event& other) const
{
    return time_ > other.time_;
}

// tutor constructor
Tutor::Tutor(const std::string& file)
    : filePassages(file)
{
}

//create the heap of events
void Tutor::createHeap()
{
    std::ifstream file("../data/" + filePassages);
    
    // does the file exist?
    if (!file)
    {
        std::cerr << "Error opening file: " << filePassages << '\n';
        return;
    }

    // the line (should be) as follows: <varco> <targa> <istante passaggio>
    std::string line;
    
    double time; // time of the event
    std::string plate;
    
    
    char delimiter = ' '; // space according to the text
	
	// helper for feedback to the user in case of error
	int lineCount = 1;
	
    while (std::getline(file, line))
    {
		//i use the istring to take the values from the file
        std::istringstream iss(line);
        
        //take varco
        int kmVarco;
        if(!(iss >> kmVarco))
        {
				std::cerr << lineCount << " row: The first value of the line is not an integer \n";
				continue;
		}
		
		//take plate of the veichle
		std::string plate;
		if(!(iss >> plate) || plate.size() != 7)
        {
				std::cerr << lineCount << " row: The second value of the line has not 7 characters \n";
				continue;
		}
		
		//take the time the vehicle passes on the varco
		double time;
		if (!(iss >> time)) {
			std::cerr << lineCount <<" row: Invalid time: not a double in row \n";
			continue; 
		}
		
		
		//create the Event
		
		Event e {kmVarco, plate, time};
		
		
		//push the Event on the heap of events
		heap.push(e);
		
		lineCount++;
		
    }
}


void Tutor::stats() const
{
    std::cout << "Number of events: " << heap.size() << '\n';

    if (!heap.empty())
    {
        std::cout << "Top score: " << heap.top().time_ << '\n';
        std::cout << "Top plate: " << heap.top().plate_ << '\n';
    }
}
