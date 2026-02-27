#pragma once

//-----------------------------------------------------------------------------
// This class represents one unloading station in the simulation.
// It maintains a queue of trucks and unloads them one at a time.
//-----------------------------------------------------------------------------

#include "common.h"
#include <queue>

class Simulation;
class Truck;
class SimulationTest;

class Station {
public:
    explicit Station(Simulation& simulation);

    void singleStep(Time now);
    void addTruckToQueue(Truck* truck, Time now);
    Time getQueueDuration(Time now);

    void printVisualization(); 
    void printStats();

private:
    Simulation& simulation_;
    std::queue<Truck*> truckQueue_;  // Queue of trucks waiting to be unloaded. We don't own the truck pointers, so we don't delete them.
    Time unloadingCompletionTime_;   // Time when current truck's unloading completes, or forever if the queue is empty

    // Accumulated statistics
    int busyMinutes_ = 0;
    int idleMinutes_ = 0;
    size_t totalQueueLength_ = 0;
    size_t maxQueueLength_ = 0;

    friend class SimulationTest;    // For testing
};

