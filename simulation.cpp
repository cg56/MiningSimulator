//-----------------------------------------------------------------------------
// This class implements the main loop of the simulation.
// It coordinates the activity of the trucks and unloading stations.
//-----------------------------------------------------------------------------

#include "simulation.h"
#include "station.h"
#include "truck.h"
#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <random>

//-----------------------------------------------------------------------------
// Constants for the visualization
//-----------------------------------------------------------------------------

const char* truckStateStr = "Truck State";
const char* stationQueueLengthStr = "Station Queue Length";
const size_t truckStateStrLen = strlen(truckStateStr);
const size_t stationQueueLengthStrLen = strlen(stationQueueLengthStr);


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Simulation::Simulation(int numTrucks, int numStations, uint32_t seed)
    : gen_(seed) {
    if ((numTrucks < 1) || (numStations < 1)) {
        throw std::invalid_argument("Number of trucks and stations must be at least 1");
    }

    trucks_.reserve(numTrucks);
    for (int i = 0; i < numTrucks; ++i) {
        trucks_.emplace_back(Truck(*this));
    }

    stations_.reserve(numStations);
    for (int i = 0; i < numStations; ++i) {
        stations_.emplace_back(Station(*this));
    }
}

//-----------------------------------------------------------------------------
// Returns a random mining duration in minutes (60–300 inclusive). This is a
// uniform distribution. In reality a normal distribution could be better.
//-----------------------------------------------------------------------------

Time Simulation::getRandomMiningDuration() {
    std::uniform_int_distribution<> dist(60, 5 * 60);  // 1–5 hours inclusive
    return static_cast<Time>(dist(gen_));
}

//-----------------------------------------------------------------------------
// Run the simulation for a given duration
//-----------------------------------------------------------------------------

void Simulation::run(Time duration) {
    printVisualizationHeader();

    for (Time now = 0; now < duration; ++now) {
        singleStep(now);
        printVisualization(now);
    }

    printVisualizationSeparator();
}

//-----------------------------------------------------------------------------
// Advance the simulation by one simulated minute
//-----------------------------------------------------------------------------

void Simulation::singleStep(Time now) {
    // Do trucks first so that they can be assigned to stations
    for (auto& truck : trucks_) {
        truck.singleStep(now);
    }

    // All available trucks are in the queues, so now process the stations
    for (auto& station : stations_) {
        station.singleStep(now);
    }
}

//-----------------------------------------------------------------------------
// Assign a truck to the station with the shortest queue duration
//-----------------------------------------------------------------------------

void Simulation::assignTruckToStation(Truck* truck, Time now) {
    assert(!stations_.empty());

    size_t shortestStation = 0;
    Time shortestDuration = stations_[0].getQueueDuration(now);

    for (size_t i = 1; i < stations_.size(); ++i) {
        Time duration = stations_[i].getQueueDuration(now);
        if (duration < shortestDuration) {
            shortestDuration = duration;
            shortestStation  = i;
        }
    }

    stations_[shortestStation].addTruckToQueue(truck, now);
}

//-----------------------------------------------------------------------------
// Calculate the width of the visualization for the trucks
//-----------------------------------------------------------------------------

size_t Simulation::trucksVisualizationWidth() {
    return std::max(trucks_.size() * 2, truckStateStrLen + 1);
}

//-----------------------------------------------------------------------------
// Calculate the width of the visualization for the stations
//-----------------------------------------------------------------------------

size_t Simulation::stationsVisualizationWidth() {
    return std::max(stations_.size() * 4, stationQueueLengthStrLen + 1);
}

//-----------------------------------------------------------------------------
// Print the separator for the visualization
//-----------------------------------------------------------------------------

void Simulation::printVisualizationSeparator() {
    std::cout << "+-------+-";

    for (size_t i = 0; i < trucksVisualizationWidth(); ++i) {
        std::cout << '-';
    }

    std::cout << "+-";

    for (size_t i = 0; i < stationsVisualizationWidth(); ++i) {
        std::cout << '-';
    }

    std::cout << "+\n";
}

//-----------------------------------------------------------------------------
// Print the header for the visualization
//-----------------------------------------------------------------------------

void Simulation::printVisualizationHeader() {
    printVisualizationSeparator();
    std::cout << std::format("| Time  | {:^{}}| {:^{}}|\n",
        truckStateStr, trucksVisualizationWidth(), stationQueueLengthStr, stationsVisualizationWidth());
    printVisualizationSeparator();
}


//-----------------------------------------------------------------------------
// Print a row of the visualization.
// First the time, then each truck's state, then each station's queue length.
//-----------------------------------------------------------------------------

void Simulation::printVisualization(Time now) {
    std::cout << "| " << formatTime(now) << " | ";

    for (Truck& truck : trucks_) {
        truck.printVisualization();
    }

    if (truckStateStrLen + 1 > trucks_.size() * 2) {
        size_t padding = truckStateStrLen + 1 - trucks_.size() * 2;
        for (size_t i = 0; i < padding; ++i)
            std::cout << ' ';
    }

    std::cout << "| ";

    for (Station& station : stations_) {
        station.printVisualization();
    }

    if (stationQueueLengthStrLen + 1 > stations_.size() * 4) {
        size_t padding = stationQueueLengthStrLen + 1 - stations_.size() * 4;
        for (size_t i = 0; i < padding; ++i)
            std::cout << ' ';
    }

    std::cout << "|\n";
}

//-----------------------------------------------------------------------------
// Print final statistics for all the trucks
//-----------------------------------------------------------------------------

void Simulation::printTruckStats() {
    const char *separator = "+-------+-------+-----------+-----------+-----------+\n";

    std::cout << '\n' << separator;
    std::cout << "| Truck | Trips | Mining    | Unloading | Traveling |\n";
    std::cout << separator;

    for (size_t i = 0; i < trucks_.size(); ++i) {
        std::cout << std::format("| {:5} |", i);
        trucks_[i].printStats();
        std::cout << '\n';
    }
    std::cout << separator;
}

//-----------------------------------------------------------------------------
// Print final statistics for all the stations
//-----------------------------------------------------------------------------

void Simulation::printStationStats() {
    const char *separator = "+---------+-----------+-----------+-------------+---------+---------+\n";

    std::cout << '\n' << separator;
    std::cout << "| Station | Idle      | Unloading | Utilization | Queue Length      |\n";
    std::cout << "|         | Minutes   | Minutes   | Percentage  | Average | Maximum |\n";
    std::cout << separator;

    for (size_t i = 0; i < stations_.size(); ++i) {
        std::cout << std::format("| {:7} |", i);
        stations_[i].printStats();
        std::cout << '\n';
    }
    std::cout << separator;
}

//-----------------------------------------------------------------------------
// Print final statistics for all the trucks and stations
//-----------------------------------------------------------------------------

void Simulation::printStats() {
    printTruckStats();
    printStationStats();
}


//-----------------------------------------------------------------------------
// Format a time as HH:MM
//-----------------------------------------------------------------------------

std::string Simulation::formatTime(Time t) {
    if (t == Forever) {
        return "-----";
    }

    int h = t / 60;
    int m = t % 60;
    return std::format("{:>2}:{:0>2}", h, m);
}
