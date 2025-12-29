//Author: Simone Piccinini

#include <string>
#include <queue>
#include <unordered_map>

// used for knowing who passes, where and when
struct Event
{
	int km_; //id of the varco
	std::string plate_; //plate of the veichle
    double time_;
    
    //I need this to manage the heap order
    bool operator<(const Event& other) const;
};


//stored information for the ticket
struct LastPassage {
    int kmVarco; //this is the km
    double time;
};


// Class
class Tutor
{
private:
	const double limit_ = 130;
	double curTime_;
	
    std::string filePassages;
    //it uses a priority queues for the events
    std::priority_queue<Event> heap;
    //dictionary to store the passed event, used for the ticket
    std::unordered_map<std::string, LastPassage> last_passage;
    
    

    

public:
    explicit Tutor(const std::string& file);
    //take the file Passages.txt and create the heap of events
    void createHeap();
    void processTickets();
    void stats() const;
    void reset();
    void set_time();
    void emit_ticket(std::string plate, double speed);
};
