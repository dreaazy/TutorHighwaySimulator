// Author: Emanuel Huber

#include "../include/Highway.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <utility>
#include <cmath>
#include <sstream>
#include <cstdlib>

const double EPS = 1e-9;  // Added for safe floating-point comparison

// Load highway from file and check all rules
Highway::Highway(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: cannot open file " << filename << std::endl;
        std::exit(1);
    }
    std::vector<std::pair<double, char>> temp;
    std::string line;
    int line_number = 0;
    
    // Read every line
    while (std::getline(file, line))
    {
        line_number++;
        if (line.empty() || line[0] == '#') continue;
        // Turn the line into a stream (For reading the km value and the type)
        std::istringstream iss(line);
        double km;
        char type;
       
        // Try to read km and type from the line
        if (!(iss >> km >> type))
        {
            std::cerr << "Line " << line_number << ": invalid format, skipped" << std::endl;
            continue;
        }
        if (km < 0)
        {
            std::cerr << "Line " << line_number << ": negative km, skipped" << std::endl;
            continue;
        }
        if (type != GATE && type != JUNCTION)
        {
            std::cerr << "Line " << line_number << ": invalid type, skipped" << std::endl;
            continue;
        }
       
        // Valid Format
        temp.push_back({km, type});
    }
    // Sort by km position
    std::sort(temp.begin(), temp.end());
    
    // Check for duplicate positions (Check that at each km there is at most one gate and one junction)
    for (size_t i = 0; i < temp.size(); )
    {
        double km = temp[i].first;
        int gates = 0;
        int junctions = 0;
        
        // Count how many gates and junctions are at this km
        for (size_t j = i; j < temp.size() && std::abs(temp[j].first - km) < EPS; ++j)
        {
            if (temp[j].second == GATE)
                gates++;
            else
                junctions++;
        }
        // Error = Two or more of the same 
        if (gates > 1 || junctions > 1)
        {
            std::cerr << "Error: multiple stations of the same type at km " << km << std::endl;
            std::exit(1);
        }
        // Jump to the next km with i
        while (i < temp.size() && std::abs(temp[i].first - km) < EPS)
            i++;
    }
    // Copy to stations
    for (const auto& p : temp)
    {
        Station s;
        s.position = p.first;
        s.type = p.second;
        stations.push_back(s);
    }
    if (stations.empty())
    {
        std::cerr << "Error: no valid stations found" << std::endl;
        std::exit(1);
    }
    // IDs setter
    int gate_count = 1;
    int junction_count = 1;
    for (auto& s : stations)
    {
        if (s.type == GATE) s.id = gate_count++;
        else s.id = junction_count++;
    }
    
    // Rule 1: at least 2 gates
    int num_gates = gate_count - 1;
    if (num_gates < 2)
    {
        std::cerr << "Error: must have at least 2 gates" << std::endl;
        std::exit(1);
    }
    
    // Find first and last gate position
    double first_gate = -1;
    double last_gate = -1;
    for (const auto& s : stations)
    {
        if (s.type == GATE)
        {
            if (first_gate == -1) first_gate = s.position;
            last_gate = s.position;
        }
    }
    
    // Rule 2: junction before first gate and after last gate
    bool junction_before = false;
    bool junction_after = false;
    for (const auto& s : stations)
    {
        if (s.type == JUNCTION)
        {
            if (s.position < first_gate) junction_before = true;
            if (s.position > last_gate) junction_after = true;
        }
    }
    if (!junction_before || !junction_after)
    {
        std::cerr << "Error: missing junction before first gate or after last gate" << std::endl;
        std::exit(1);
    }
    // Rule 3: minimum 1 km between gate and junction (check consecutive different types)
    for (size_t i = 1; i < stations.size(); ++i)
    {
        if (stations[i].type != stations[i-1].type)
        {
            if (stations[i].position - stations[i-1].position < 1.0)
            {
                std::cerr << "Error: distance less than 1 km between "
                          << stations[i-1].position << " (" << stations[i-1].type << ") and "
                          << stations[i].position << " (" << stations[i].type << ")" << std::endl;
                std::exit(1);
            }
        }
    }
}
const std::vector<Station>& Highway::getStations() const
{
    return stations;
}
const Station* Highway::findClosestStation(double km) const
{
    if (stations.empty()) return nullptr;
    const Station* closest = &stations[0];
    double min_distance = std::abs(stations[0].position - km);
    for (const auto& s : stations)
    {
        double distance = std::abs(s.position - km);
        if (distance < min_distance)
        {
            min_distance = distance;
            closest = &s;
        }
    }
    return closest;
}
std::vector<Station> Highway::getGates() const
{
    std::vector<Station> gates;
    for (const auto& s : stations)
    {
        if (s.type == GATE) gates.push_back(s);
    }
    return gates;
}
std::vector<Station> Highway::getJunctions() const
{
    std::vector<Station> junctions;
    for (const auto& s : stations)
    {
        if (s.type == JUNCTION) junctions.push_back(s);
    }
    return junctions;
}
