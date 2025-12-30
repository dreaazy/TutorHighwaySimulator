#include "tutor.h"

int main()
{
	
    Tutor tutor("Passages.txt");

    
    tutor.set_time(9000);
    
    tutor.stats();
    
    tutor.reset();
    
    tutor.set_time(9000);
    
    tutor.stats();
    
    
}
