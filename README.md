# Mining Simulator
This project implements a simulation for a lunar Helium-3 space mining
operation. The simulation manages and tracks the efficiency of mining trucks
and unload stations over a continuous 72-hour operation.

# Build and run
The miner can be built and run as follows:
g++ -std=c++20 simulation.cpp station.cpp truck.cpp main.cpp -o miner && ./miner 10 3

The testcases can be built and run as follows:
g++ -std=c++20 simulation.cpp station.cpp truck.cpp test.cpp -o test && ./test

The miner accepts two command-line arguments. The first is the number of trucks
to simulate and the second is the number of unloading stations to simulate.

# Key Design Notes
* Time is updated with a minimum resolution of one minute.
* The simulation needs to run for 72 hours (4,320 minutes). So with one
minute resolution, uint32_t is big enough to hold time values.
* The simulation class keeps track of the passage of time and updates each
truck and station on each minute.
* The truck and station classes have completion time member variables. These
keep track of how long the current state or activity is to be executed for.
* The command-line processing is very simple. It could be extended with
additional arguments to specify the simulation duration, transit times,
verbosity of output, etc. Ideally this would use the argparse package, but
that is not standard on my system.

# Discussion Points
The simulation class basically just keeps track of time, all the coordination
of trucks arriving and unloading at stations is implemented in the truck and
station classes. An alternative approach would be for the truck and station
classes to be dumber and the simulation class would take care of all the
coordination. However, if the simulation is extended with other types of
elements, then having centralized business logic could become complex and the
simulator class would become more of a god object. On the other hand though,
centralized logic can be easier to debug if there are unexpected interactions
between classes.

The current time, referred to as "now" in the code, is passed around from
one method to another. This could be moved to be a member variable of the
Simulator class, but this would increase coupling.

With the current implementation, given n trucks and m stations, assigning
trucks to the station with the shortest duration is at worse case O(n x m).
Typically it will be less as only some trucks arrive at the stations during
each simulation step. However, for large simulations this could be improved
by, for example, caching and pre-sorting the station queue durations.

The State enum class uses an int as the underlying type. If memory space is an
issue then this could be reduce to a short or even a char type.

A visualization of the current state of each truck and the queue length of each
station is printed out after each simulation step. This becomes unreadable if
there are a large number of trucks or stations and should likely have a flag
to enable/disable the output.

Some statistics are printed out when the simulation completes. There are likely
many more that could be useful to calculate.
