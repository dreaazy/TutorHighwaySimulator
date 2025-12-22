#include "tutor.h"

int main()
{
    Tutor tutor("Passages.txt");
    tutor.createHeap();
    tutor.stats();
}
