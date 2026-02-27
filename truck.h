#pragma once

//-----------------------------------------------------------------------------
// This class represents one mining truck in the simulation.
// It moves through a fixed cycle of mining, returning, unloading, and going
// back to a mine.
//-----------------------------------------------------------------------------

#include "common.h"
#include <array>

class Simulation;
class SimulationTest;

class Truck {
public:
    explicit Truck(Simulation& simulation);

    void singleStep(Time now);
    void unloadingCompleted(Time now); 

    void printVisualization();
    void printStats(); 

private:
    enum class State : int {
        Outgoing,   // Traveling to the mine
        Mining,
        Incoming,   // Traveling back to unload
        Unloading,
        NumStates   // Must be the last enum value
    } state_;

    Simulation& simulation_;
    Time stateCompletionTime_;  // Minute at which current state ends (or forever if waiting)

    // Accumulated statistics
    std::array<int, static_cast<int>(State::NumStates)> totals_{0};     // Minutes spent in each state
    int roundTrips_ = 0;    // Number of roundtrips (to and from the mine) that the truck has done

    friend class SimulationTest;    // For testing
};
