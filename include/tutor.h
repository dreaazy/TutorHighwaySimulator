//Author: Simone Piccinini

#include "./array_view.h"

#include <string>
#include <queue>
#include <unordered_map>
#include <vector>

#include "../include/Highway.h"

// used for knowing who passes, where and when
struct Event
{
	int id; //id of the varco
	std::string plate_; //plate of the vehicle
    double time_;
    
    //I need this to manage the heap order
    bool operator<(const Event& other) const;
};


//stored information for the ticket
struct LastPassage {
    int id;
    double time;
};

struct VarcoStats
{
    int totalVehicles = 0;
};


// Class


class Tutor
{
private:
	const double limit_ = 130;
	double curTime_ = 0; // the timeline start at time 0
	
	
	//attributes used for stats used to compute the average speed and stats
	int totalDistance_ = 0;
	double totalTime_ = 0;
	int santionedVehicles_ = 0;
	
	//stations
	std::vector<Station> stationStorage_; // owns data
	ArrayView<Station> stations_; // view
	
	
    std::string filePassages;
    //it uses a priority queues for the events
    std::priority_queue<Event> heap;
    //dictionary to store a passed event, of a STILL OPENED ROUTE
    std::unordered_map<std::string, LastPassage> last_passage;
    //used for stats of routes
    std::unordered_map<int, VarcoStats> varcoStats_;

    

public:
    explicit Tutor(const std::string& file);
    //take the file Passages.txt and create the heap of events
    void createHeap();
    void processEvent(Event e);
    void stats() const;
    void reset();
    //it takes a string, 12 are seconds, 12m are minutes
    void set_time(const std::string& t);
    //used to print on terminal the tickets
    void emit_ticket(const std::string &plate, double speed, int ident, double entVarco, double entTime, int idend, double endVarco, double endTime);
    
};
