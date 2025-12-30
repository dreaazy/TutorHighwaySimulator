#include "tutor.h"

int main()
{
	
    Tutor tutor("Passages.txt");
    
    tutor.createHeap();
    tutor.stats();
    tutor.set_time(50);
    
    
}
