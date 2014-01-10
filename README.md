# Pithos

Pithos represents the implementation work of my PhD under [Dr. Herman Engelbrecht](http://ml.sun.ac.za/people/dr-ha-engelbrecht/) at the [Stellenbosch University](http://www.sun.ac.za/english)'s [MIH Media lab](http://ml.sun.ac.za/).

This repository contains Pithos source code which allows you to simulate a distributed object store for P2P MMVEs in [Oversim](http://www.oversim.org/), runninng on [Omnet++](http://www.omnetpp.org/). It also contains all data analysis scripts that I used to generate the results for my PhD.

For more information on Pithos, have a look at my [thesis](http://scholar.sun.ac.za/handle/10019.1/80268).

## Using Pithos
Pithos is implemented as an Oversim simulation, with some modifications to Oversim itself. The directory structure matches the Oversim directory structure. To start using Pithos, merge the Oversim and Pithos Oversim folders.

Before trying to use Pithos, make sure you have read everything there is to read about Oversim (which isn't a lot).

## Oversim resources
1. [Oversim documentation page](http://www.oversim.org/wiki/OverSimGuide)
2. [Oversim demystified](https://sites.google.com/site/oversimdemystified/)

## Pithos todo list:

If anyone is interested, this is the way to take Pithos further.

### Major
1. Implment Pithos as a real world application
2. Develop grouping algorithm based on player traces
3. Have Pithos construct groups based on the grouping algorithm
4. Integrate Pithos into an existing P2P MMOG

### Minor
1. Implement the designed object modification for group storage.
2. Extend the pithos test application to add modify testing.

### Really minor
1. Make the latitude and longitude ranges for peers and super peers a changeable paramter instead of 100
2. Support can be added for partially connected groups that will reduce network bandwidth at the cost of latency
3. Add a debug mode to the code, which ads the debugging code currently present in Pithos as an option and not always.
4. Group storage should record its own successes, failures and latencies and not depend on PithosTestApp from recording them indirectly.
5. The DHT module can be changed so as to store and retrieve GameObjects instead of BinaryValues
6. A maximum distance needs to be defined for the directory server
7. Ensure correctness for pithos under 32bit systems. This includes testing long variable sizes and making them 32bit safe. In most scenarios, Pithos, uses more than 3 GB of RAM, which means 32 bit architectures cannot be used.
