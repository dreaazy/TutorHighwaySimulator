#pragma once

#include <string>
#include <queue>

// Data type
struct Event
{
	int km_; //id of the varco
	std::string plate_; //plate of the veichle
    double time_;
    
    //I need this to manage the heap order
    bool operator<(const Event& other) const;
};

// Class
class Tutor
{
private:
    std::string filePassages;
    std::priority_queue<Event> heap;

public:
    explicit Tutor(const std::string& file);
    void createHeap();
    void stats() const;
};
