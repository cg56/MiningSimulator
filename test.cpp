//-----------------------------------------------------------------------------
// This file contains test cases for Simulation, Station, and Truck classes.
//-----------------------------------------------------------------------------

#include "common.h"
#include "simulation.h"
#include "station.h"
#include "truck.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Macros for assertions
//-----------------------------------------------------------------------------

#define ASSERT_TRUE(func) do { \
    if (!(func)) { \
        std::cerr << "Test failed: " << #func << "\n"; \
        exit(1); \
    } \
} while (0)

#define ASSERT_THROWS(func) do { \
    try { \
        (func); \
        std::cerr << "Test failed to throw expected exception: " << #func << "\n"; \
        exit(1); \
    } catch (...) { \
        /* expected */ \
    } \
} while (0)

#define ASSERT_NO_THROW(func) do { \
    try { \
        (func); \
    } catch (...) { \
        std::cerr << "Test failed: " << #func << "\n"; \
        exit(1); \
    } \
} while (0)

//-----------------------------------------------------------------------------
// This SimulationTest class is a  friend of the Simulation, Truck, Station
// classes which allows direct access for unit tests
//-----------------------------------------------------------------------------

class SimulationTest {
public:
    //-----------------------------------------------------------------------------
    // Test the constructor of the Simulation class. It should throw an exception
    // if the number of trucks or stations is less than 1.
    //-----------------------------------------------------------------------------

    static void testSimulatorConstructor() {
        ASSERT_THROWS(Simulation(-1, 5, 56));
        ASSERT_THROWS(Simulation(0, 5, 56));
        ASSERT_THROWS(Simulation(5, -1, 56));
        ASSERT_THROWS(Simulation(5, 0, 56));
        ASSERT_NO_THROW(Simulation(5, 2, 56));
    }

    //-----------------------------------------------------------------------------
    // Test the getRandomMiningDuration method of the Simulation class.
    // It should return a random duration between 60 and 300 minutes.
    //-----------------------------------------------------------------------------

    static void testRandomDuration() {
        Simulation simulator(1, 1, 56);

        for (int i = 0; i < 1000; ++i) {
            Time d = simulator.getRandomMiningDuration();
            ASSERT_TRUE((d >= 60) && (d <= 5 * 60));
        }
    }

    //-----------------------------------------------------------------------------
    // Test the states of the Truck class.
    // It should start in the mining state and transition through the other states.
    //-----------------------------------------------------------------------------

    static void testTruckStates() {
        Simulation simulator(1, 1, 56);

        simulator.run(1);
        ASSERT_TRUE(getTruckStateTotal(simulator, 0) == 0);
        ASSERT_TRUE(getTruckStateTotal(simulator, 1) > 0);    // Should start in the mining state
        ASSERT_TRUE(getTruckStateTotal(simulator, 2) == 0);
        ASSERT_TRUE(getTruckStateTotal(simulator, 3) == 0);
        ASSERT_TRUE(simulator.trucks_[0].roundTrips_ == 0);

        simulator.run(7*60);  // Long enough for the truck to transition through every state
        ASSERT_TRUE(getTruckStateTotal(simulator, 0) > 0);
        ASSERT_TRUE(getTruckStateTotal(simulator, 1) > 0);
        ASSERT_TRUE(getTruckStateTotal(simulator, 2) > 0);
        ASSERT_TRUE(getTruckStateTotal(simulator, 3) > 0);
        ASSERT_TRUE(simulator.trucks_[0].roundTrips_ > 0);
    }

    //-----------------------------------------------------------------------------
    // Test the queue length of the Station class.
    // It should start at 0 and increase as trucks are added to the queue.
    //-----------------------------------------------------------------------------

    static void testQueueLength() {
        Simulation simulator(1, 1, 56);

        simulator.run(1);
        ASSERT_TRUE(simulator.stations_[0].idleMinutes_ == 1);
        ASSERT_TRUE(simulator.stations_[0].busyMinutes_ == 0);
        ASSERT_TRUE(simulator.stations_[0].totalQueueLength_ == 0);
        ASSERT_TRUE(simulator.stations_[0].maxQueueLength_ == 0);

        simulator.run(7*60);  // Long enough for the truck to transition through every state
        ASSERT_TRUE(simulator.stations_[0].idleMinutes_ > 0);
        ASSERT_TRUE(simulator.stations_[0].busyMinutes_ > 0);
        ASSERT_TRUE(simulator.stations_[0].totalQueueLength_ > 0);
        ASSERT_TRUE(simulator.stations_[0].maxQueueLength_ == 1);
    }

    //-----------------------------------------------------------------------------
    // Test the duration of the Simulation class.
    // It should run for the specified duration and the total time for each station
    // should be equal to the duration.
    //-----------------------------------------------------------------------------

    static void testDuration() {
        constexpr int NumStations = 10;
        constexpr int Duration = 72*60;

        Simulation simulator(100, NumStations, 56);

        simulator.run(Duration);
        for (int i = 0; i < NumStations; ++i) {
            ASSERT_TRUE(simulator.stations_[i].idleMinutes_ + simulator.stations_[i].busyMinutes_ == Duration);
        }
    }

private:
    //-----------------------------------------------------------------------------
    // Helper method to get the number of times a truck has been in each state
    //-----------------------------------------------------------------------------

    static int getTruckStateTotal(Simulation& simulator, int state) {
        return simulator.trucks_[0].totals_[state];
    }
};

//-----------------------------------------------------------------------------
// Main entry point for the test cases
//-----------------------------------------------------------------------------

int main() {
    SimulationTest::testSimulatorConstructor();
    SimulationTest::testRandomDuration();
    SimulationTest::testTruckStates();
    SimulationTest::testQueueLength();
    SimulationTest::testDuration();

    std::cout << "All tests passed\n";
    return 0;
}
