#pragma once

//-----------------------------------------------------------------------------
// Shared types and constants for the mining simulator
//-----------------------------------------------------------------------------

#include <cstdint>
#include <limits>

// All time is measured in minutes since the start of the simulation. uint32_t is enough for 72 hours
using Time = uint32_t;

// Forever is used to wait forever, i.e: no set completion time 
constexpr Time Forever = std::numeric_limits<Time>::max();

constexpr Time SimulationDuration = 72 * 60;    // Default run length (in minutes) is 72 hours 
constexpr Time TravelingDuration = 30;          // Travel time between a mine and a station and vise-versa
constexpr Time UnloadingDuration = 5;           // Time to unload a truck at a station