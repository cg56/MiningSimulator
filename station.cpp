//-----------------------------------------------------------------------------
// The queue implementation for one station
//-----------------------------------------------------------------------------

#include "common.h"
#include "simulation.h"
#include "station.h"
#include "truck.h"
#include <format>
#include <iostream>


//-----------------------------------------------------------------------------
// The stations starts off empty and will ait forever until a truck arrives
//-----------------------------------------------------------------------------

Station::Station(Simulation& simulation) : simulation_(simulation){
    unloadingCompletionTime_ = Forever;
}

//-----------------------------------------------------------------------------
// Advance the station by one simulated minute
//-----------------------------------------------------------------------------

void Station::singleStep(Time now) {
    // Has the current truck's unloading completed?
    if (now >= unloadingCompletionTime_) {
        Truck* unloadingTruck = truckQueue_.front();
        unloadingTruck->unloadingCompleted(now);
        truckQueue_.pop();

        // How long until we check again?
        if (truckQueue_.empty()) {
            unloadingCompletionTime_ = Forever;   // Wait forever until a truck arrives
        } else {
            unloadingCompletionTime_ = now + UnloadingDuration;
        }
    }

    // Update the statistics
    if (truckQueue_.empty()) {
        ++idleMinutes_;
    } else {
        ++busyMinutes_;
        totalQueueLength_ += truckQueue_.size();
        maxQueueLength_ = std::max(maxQueueLength_, truckQueue_.size());
    }
}

//-----------------------------------------------------------------------------
// Add a truck to the station's queue
//-----------------------------------------------------------------------------

void Station::addTruckToQueue(Truck* truck, Time now) {
    // If the station is free then start unloading immediately
    if (truckQueue_.empty()) {
        unloadingCompletionTime_ = now + UnloadingDuration;
    }

    truckQueue_.push(truck);
}

//-----------------------------------------------------------------------------
// Caculates the number of minutes for the station's queue to be empty.
// Used by the simulator to pick the shortest queue.
//-----------------------------------------------------------------------------

Time Station::getQueueDuration(Time now) {
    if (truckQueue_.empty()) {
        return 0;
    }

    Time duration = UnloadingDuration + (unloadingCompletionTime_ - now);   // How much time remaining to unload the current truck?
    duration += (truckQueue_.size() - 1) * UnloadingDuration;   // Plus all the other trucks in the queue
    return duration;
}

//-----------------------------------------------------------------------------
// Print a visualization of the current state of the station's queue
//-----------------------------------------------------------------------------

void Station::printVisualization() {
    std::cout << std::format("{:3} ", truckQueue_.size());
}

//-----------------------------------------------------------------------------
// Print some useful statistics of the station's queue
//-----------------------------------------------------------------------------

void Station::printStats() {
    double duration = idleMinutes_ + busyMinutes_;
    double utilization = 0.0;
    double averageQueueLength = 0.0;

    if (duration > 0) {
        utilization = busyMinutes_ / duration * 100.0;
        averageQueueLength = totalQueueLength_ / duration;
    }

    std::cout << std::format("{:10} | {:9} | {:>10.1f}% | {:7.2f} | {:7} |",
        idleMinutes_, busyMinutes_, utilization, averageQueueLength, maxQueueLength_);
}
