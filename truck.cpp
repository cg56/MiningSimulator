//-----------------------------------------------------------------------------
// The state machine and stats for one mining truck
//-----------------------------------------------------------------------------

#include "truck.h"
#include "simulation.h"
#include <format>
#include <iostream>

//-----------------------------------------------------------------------------
// Trucks start in the mining state
//-----------------------------------------------------------------------------

Truck::Truck(Simulation& simulation) : simulation_(simulation) {
    state_ = State::Mining;
    stateCompletionTime_ = simulation_.getRandomMiningDuration();
}

//-----------------------------------------------------------------------------
// Advances the truck by one simulated minute
//-----------------------------------------------------------------------------

void Truck::singleStep(Time now) {
    ++totals_[static_cast<int>(state_)];

    if (now < stateCompletionTime_) {
        return; // Get out quick
    }

    // Move to the next state
    switch (state_) {
    case State::Outgoing:
        state_ = State::Mining;
        stateCompletionTime_ = now + simulation_.getRandomMiningDuration();
        break;
    case State::Mining:
        state_ = State::Incoming;
        stateCompletionTime_ = now + TravelingDuration;
        break;
    case State::Incoming:
        state_ = State::Unloading;
        stateCompletionTime_ = Forever;   // Wait forever until we are told that we are finished unloading
        simulation_.assignTruckToStation(this, now);
        break;
    case State::Unloading:
        // There is no transition from this state, a station will call unloadingCompleted() to move on
        break;
    }
}

//-----------------------------------------------------------------------------
// Called by a station when it finishes unloading this truck.
// The truck will transition back to the outgoing state.
//-----------------------------------------------------------------------------

void Truck::unloadingCompleted(Time now) {
    state_ = State::Outgoing;
    stateCompletionTime_ = now + TravelingDuration;
    ++roundTrips_;
}

//-----------------------------------------------------------------------------
// Print a visualization of the current state of the truck
//-----------------------------------------------------------------------------

void Truck::printVisualization() {
    static const std::array<char, static_cast<int>(State::NumStates)> stateChars{'O', 'M', 'I', 'U'};

    std::cout << stateChars[static_cast<int>(state_)] << ' ';
}

//-----------------------------------------------------------------------------
// Print some useful statistics of the truck's activities
//-----------------------------------------------------------------------------

void Truck::printStats() {
    int mining = totals_[static_cast<int>(State::Mining)];
    int unloading = totals_[static_cast<int>(State::Unloading)];
    int traveling = totals_[static_cast<int>(State::Outgoing)] + totals_[static_cast<int>(State::Incoming)];

    std::cout << std::format("{:6} | {:9} | {:9} | {:9} |",
        roundTrips_, mining, unloading, traveling);
}
