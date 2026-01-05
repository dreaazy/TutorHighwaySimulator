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
#include <algorithm>


// I want a min heap, because I want to extract before the smallest event
// priority queue uses < to compare elements, according to time
bool Event::operator<(const Event& other) const
{
    return time_ > other.time_;
}

// tutor constructor
Tutor::Tutor(const std::string& file) : filePassages(file)
{
	//read highway and populate the Stations' array, using ArrayView for controlled access
	
	//take the stations from file
	Highway hw;
	stationStorage_ = hw.getStations();
	//sort them
	std::sort(stationStorage_.begin(), stationStorage_.end());
	stations_ = ArrayView<Station>(stationStorage_.data(), static_cast<int>(stationStorage_.size()));
	
	//create heap of events from passages
	createHeap();
	
	
}


//process only one event, called inside set_time
void Tutor::processEvent(Event e)
{
	//update stats of varco
	varcoStats_[e.id].totalVehicles++;
	
	//take data of the car that JUST PASSED THE STATION
	double currentTime = e.time_;
	std::string currentPlate = e.plate_;
	
	//get the km of the station from the station id
	const Station& currentStation = stations_[e.id];
	double currentKmStation = currentStation.position;
	
				
				
	//check the car is already passed
	auto it = last_passage.find(currentPlate);
				
	//if it find something it returns the iterator in that position
	//if not it returns container.end()
	if(it == last_passage.end())
	{
		//take the id of the station
		
		//the car is not yet passed in from of a tutor, insert in the dictionary
		last_passage[currentPlate] ={e.id,currentTime};
	}
	else//the car was passed in from of a tutor 
	{
		//access with the iterator the LastPassage object as reference
		LastPassage& previous = it->second;
		
		//taking km of previous station
		const Station& previousStation = stations_[previous.id];
		double previousKmStation = previousStation.position;
		//taking time
		double previousTime = previous.time;
		
			
		//data
		double space = std::abs(currentKmStation - previousKmStation);
		
		//the time is in seconds
		double time = std::abs(currentTime - previousTime); 
		
		//convert to hours
		time = time / 3600;
		
		double vMedia = space / time;
					
		if(vMedia > limit_)
		{
			//count of sanctioned vehicles
			santionedVehicles_++;
			
		//to be printed
		//plate - average velocity - start varco - start time - end varco - end time
			emit_ticket(currentPlate, vMedia, previousKmStation, previousTime, currentKmStation, currentTime);
						
		}
		
		
		//update the total time and total distance
		totalTime_ += time;
		totalDistance_ +=space;
		
		
		last_passage.erase(it);	
					
	}
	
				
				
}

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
    
    char delimiter = ' '; // space according to the text
	
	// helper for feedback to the user in case of error
	int lineCount = 1;
	
    while (std::getline(file, line))
    {
		// i use the istring to take the values from the file
        std::istringstream iss(line);
        
        // id of the varco
        int id;
        if (!(iss >> id))
        {
            std::cerr << lineCount << " row: The first value of the line is not a int\n";
            lineCount++;
            continue;
        }
		
		// take plate of the vehicle
		std::string plate;
		if (!(iss >> plate) || plate.size() != 7)
        {
            std::cerr << lineCount << " row: The second value of the line has not 7 characters\n";
            lineCount++;
            continue;
		}
		
		// take the time the vehicle passes on the varco
		double time;
		if (!(iss >> time))
        {
			std::cerr << lineCount << " row: Invalid time: not a double\n";
			lineCount++;
			continue; 
		}
		
		// create the Event
		Event e {id, plate, time};
		
		// push the Event on the heap of events
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


