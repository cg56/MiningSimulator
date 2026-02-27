#include "simulation.h"
#include <iostream>
#include <random>

//-----------------------------------------------------------------------------
// Main entry point for the simulation
// Command-line arguments: <numTrucks> <numStations>
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <numTrucks> <numStations>\n";
        return 1;
    }

    try {
        int numTrucks = std::stoi(argv[1]);
        int numStations = std::stoi(argv[2]);

        std::random_device rd;
        uint32_t seed = rd();

        Simulation simulator(numTrucks, numStations, seed);
        simulator.run(SimulationDuration);
        simulator.printStats();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
