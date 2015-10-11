# daylite

Daylite is the communications backbone for a number of processes running on the
next-generation Botball robot controller from [KIPR](http://kipr.org/). Its design is inspired by
[ROS](http://www.ros.org/). Daylite, however, is simple to build, install, and use in comparison, with
no external dependencies or extra tooling required. It also makes several different design choices.

Daylite is named after the song [Daylight by Andrew Rayel](https://www.youtube.com/watch?v=wmoyqXaWT0g).
It was the song that was playing when I decided I needed to make this. Unfortunately, the variable
`daylight` is defined in `time.h` and as such cannot be used for a namespace.

## Design

Daylite is best represented as a fully connected graph of nodes. Nodes advertise
topics that they listen to and publish data in the form of binary JSON to topics.
Nodes may be in the same process as one-another, on the same machine, over a LAN,
or even over the internet. Nodes currently communicate over TCP, though this is relatively
straightforward to change.

Unlike ROS, Daylite is schemaless by design. Any well-formed data may be published
to a topic. It is up to the recipient to discern a message's contents. This is not
necessarily the intended use-case, however. It is merely to simplify library usage,
dependencies, and build process at the modest expense of safety and performance.

Also unlike ROS, Daylite does not have a well-defined master node. While there currently
is no peer auto-discovery, once a node establishes a connection to another node it becomes
a member of the entire daylite network graph. This allows the graph to grow rather organically.

Daylite is designed to run on any OS with BSD-compliant sockets (including Windows, Mac OS, and Linux).
It does not take advantage of any un-portable libraries or language features.

## Future work

In the future, Daylite will support automatically extending its node graph through network meshing.
This will allow robot controllers to seamlessly communicate.


Requirements
============

* CMake 2.8.0 or higher
* libbson
* boost

Building (OS X and Linux)
=========================

    cd daylite
    mkdir build
    cd build
    cmake ..
    make
    make install

## License

Daylite is released under the terms of the GNU General Public License Version 3 (GPLv3). For more details, see the
`LICENSE` file in the root of the project.
