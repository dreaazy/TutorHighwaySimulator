#include "Tutor.h"

int main()
{
    Tutor tutor("passages.txt");
    tutor.createHeap();
    tutor.stats();
    tutor.setTime("20m");
}
