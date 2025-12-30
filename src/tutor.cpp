//Author: Simone Piccinini

//used a min-heap for managing the events
//this allows to have O(log n) in extracting the minor time
//only the set_time can extract from the heap
//used an unordered_map<plate, LastPassage> to save the last time a car X passed in front of a tutor, so the next time I extract event of car X I can compute it's avg speed

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
	createHeap();
}


//process only one event, called inside set_time
void Tutor::processEvent(Event e)
{
	//update stats of varco
	varcoStats_[e.km_].totalVehicles++;
	
	double passedTime = e.time_;
	int passedKm = e.km_; //this is also the id of the varco
	std::string passedPlate = e.plate_;
				
				
	//check the car is already passed
	auto it = last_passage.find(passedPlate);
				
	//if it find something it returns the iterator in that position
	//if not it returns container.end()
	if(it == last_passage.end())
	{
		//the car is not yet passed in from of a tutor
		last_passage[passedPlate] ={passedKm,passedTime};
	}
	else//the car was passed in from of a tutor 
	{
		//access with the iterator the LastPassage object as reference
		LastPassage& last = it->second;
					
		//data
		int kmDone = std::abs(e.km_ - last.kmVarco);
		
		//the time is in seconds
		double timeDone = e.time_ - last.time; 
		timeDone = timeDone / 3600;
		
		double vMedia = kmDone / timeDone;
					
		if(vMedia > limit_)
		{
			//count of sanctioned vehicles
			santionedVehicles_++;
			
		//to be printed
		//plate - average velocity - start varco - start time - end varco - end time
			emit_ticket(passedPlate, vMedia, last.kmVarco, last.time, passedKm, passedTime);
						
		}
		
		
		//update the total time and total distance
		totalTime_ += timeDone;
		totalDistance_ +=kmDone;
		
		
		last_passage.erase(it);	
					
	}
	
				
				
}

//process all tickets in the heap 
void Tutor::processTickets()
{
		// first check if the heap is empty
		
		while(!heap.empty())
		{
				Event e = heap.top();
				heap.pop();
				double passedTime = e.time_;
				int passedKm = e.km_; //this is also the id of the varco
				std::string passedPlate = e.plate_;
				
				
				//check the car is already passed
				auto it = last_passage.find(passedPlate);
				
				//if it find something it returns the iterator in that position
				//if not it returns container.end()
				if(it == last_passage.end())
				{
						//the car is not yet passed in from of a tutor
						last_passage[passedPlate] ={passedKm,passedTime};
				}
				else//the car was passed in from of a tutor 
				{
					//access with the iterator the LastPassage object as reference
					LastPassage& last = it->second;
					
					//data
					int kmDone = e.km_ - last.kmVarco;
					double timeDone = e.time_ - last.time;
					double vMedia = kmDone / timeDone;
					
					if(vMedia > limit_)
					{
						//to be printed
						//plate - average velocity - start varco - start time - end varco - end time
						emit_ticket(passedPlate, vMedia, last.kmVarco, last.time, passedKm, passedTime);
						
					}
					else
					{
							last = {passedKm, passedTime};
					}
					
					
				}
				
			
		}
	
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
	std::cout << "---------------------------------------------------" <<"\n";
	std::cout << "Stats of vehicles until time: " << curTime_ << "\n";
    for(auto& p : varcoStats_)
    {
		int varco = p.first;
		const VarcoStats& s = p.second;
		
		double minutes = curTime_ / 60.0;
		int avgPerMin = minutes > 0 ? s.totalVehicles / minutes : 0;
		
		std::cout << "Varco "<< varco << ": "
				  << s.totalVehicles << " veicoli, "
				  << avgPerMin << " veicoli/min \n";
				  
	}
	
	double avgSpeed = totalTime_ > 0 ? totalDistance_ / totalTime_ : 0;
	
	std::cout << "velocita media: " << avgSpeed << "\n";
	std::cout << "veicoli sanzionati: " << santionedVehicles_ << "\n";
}


void Tutor::emit_ticket(const std::string plate,
                        double speed,
                        int entrVarco,
                        double entrTime,
                        int endVarco,
                        double endTime)
{
    std::cout << "Plate: " << plate
              << " | Speed: " << speed << " km/h"
              << " | Varco Entrata: " << entrVarco << " km"
              << " at " << entrTime << " sec."
              << " | Varco Uscita: " << endVarco << " km"
              << " at " << endTime << " sec."
              << '\n';
}




void Tutor::set_time(double newTime)
{
	//for better reading
	std::cout << "---------------------------------------------------" <<"\n";
	//check with current time
	if(newTime < curTime_)
	{
		//error the time inserted cannot be previous of the last one
		std::cout << "You cannot go back int time";
		
	}
	else
	{
		//update the heap to consider the events in the right interval  of time
		while(!heap.empty() && newTime > heap.top().time_)
		{	
			Event e = heap.top();
			heap.pop();
			
			processEvent(e);
		}
		
		//after updated print veichles that have surpassed the speed limit
		
		//processTickets(); 
		
		curTime_ = newTime;
	}
	
}


void Tutor::reset()
{
		
		
		//default
		curTime_ = 0;
		totalDistance_ = 0;
		totalTime_ = 0;
		santionedVehicles_ = 0;
		
		//clear maps
		last_passage.clear();
		varcoStats_.clear();
		
		//svuoto heap
		heap = std::priority_queue<Event>();
		//create again the heap
		createHeap();
		
	
}
