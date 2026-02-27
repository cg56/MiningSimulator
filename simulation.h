#pragma once

//-----------------------------------------------------------------------------
// This class implements the main loop of the simulation.
// It coordinates the activity of the trucks and unloading stations.
//-----------------------------------------------------------------------------

#include "common.h"
#include "station.h"
#include "truck.h"
#include <random>
#include <string>
#include <vector>

class SimulationTest;

class Simulation {
public:
    Simulation(int numTrucks, int numStations, uint32_t seed);

    void run(Time duration);
    void assignTruckToStation(Truck *truck, Time now);
    void printStats();

    Time getRandomMiningDuration();

private:
    void singleStep(Time now);

    void printVisualization(Time now);
    void printVisualizationHeader();
    void printVisualizationSeparator();
    void printTruckStats();
    void printStationStats();
    size_t trucksVisualizationWidth();
    size_t stationsVisualizationWidth();

    static std::string formatTime(Time t);

    std::vector<Truck> trucks_;     // All the trucks in the sumulation
    std::vector<Station> stations_; // All the unloading stations in the simulation
    std::mt19937 gen_;              // Random number generator

    friend class SimulationTest;
};

