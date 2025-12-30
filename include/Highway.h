// Highway.h - Header file for the Highway class.
// Author: Emanuel Huber

#ifndef HIGHWAY_H
#define HIGHWAY_H

#include <vector>
#include <string>

// Constants for station types - keeping it simple with chars.
const char GATE = 'V';     // 'V' for Gate (Varco)
const char JUNCTION = 'S'; // 'S' for Junction (Svincolo)

// Struct for a station on the highway.
struct Station {
    double position; // Position in km from origin - should be >= 0.
    char type;       // 'V' or 'S'.
    int id;          // ID starting from 1, separate for gates and junctions.
};

// Class for the highway - loads from file, sorts, assigns IDs, and checks validity.
class Highway {
private:
    std::vector<Station> stations;

public:
    // Constructor - loads the file, processes data, and validates.
    Highway(const std::string& filename = "../data/Highway.txt");

    // Get all stations.
    const std::vector<Station>& getStations() const;

    // Find the closest station to a given position.
    const Station* findClosestStation(double km) const;

    // Get only gates.
    std::vector<Station> getGates() const;

    // Get only junctions.
    std::vector<Station> getJunctions() const;
};

#endif // HIGHWAY_H
