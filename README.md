John Gilmore PhD repo
=====================

This is the PhD repository of John Gilmore. It contains Pithos, which is the Oversim simulation I implemented for my PhD. It also contains the LaTeX code and other resources for all papers I wrote while I was doing my PhD. Lastly, any presentations I made are also contained herein.

Pithos todo list:
=================

If anyone is interested, this is the way to take Pithos further.

Major:
Implment Pithos as a real world application
Develop grouping algorithm based on player traces
Have Pithos construct groups based on the grouping algorithm
Integrate Pithos into an existing P2P MMOG
Write journal paper on Pithos.
Write journal paper on expected object lifetimes.
Write journal paper on Generic consistency model for MMVEs.

Minor:
Implement the designed object modification for group storage.
Extend the pithos test application to add modify testing.

Really minor:
Make the latitude and longitude ranges for peers and super peers a changeable paramter instead of 100
Support can be added for partially connected groups that will reduce network bandwidth at the cost of latency
Add a debug mode to the code, which ads the debugging code currently present in Pithos as an option and not always.
Group storage should record its own successes, failures and latencies and not depend on PithosTestApp from recording them indirectly.
The DHT module can be changed so as to store and retrieve GameObjects instead of BinaryValues
A maximum distance needs to be defined for the directory server
Ensure correctness for pithos under 32bit systems. This includes testing long variable sizes and making them 32bit safe. In most scenarios, Pithos, uses more than 3 GB of RAM, which means 32 bit architectures cannot be used.
