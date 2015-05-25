# daylite

Daylite is the communications backbone for a number of processes running on the
next-generation Botball robot controller from KIPR. It's design was inspired by
ROS. Daylite, however, is simple to build, install, and use in comparison, with
no external dependencies or extra tooling required.

## Design

Daylite is best represented as a fully connected graph of nodes. Nodes advertise
topics that they listen to and publish data in the form of binary JSON to topics.
Nodes may be in the same process as one-another, on the same machine, over a LAN,
or even over the internet. Nodes currently communicate over TCP.

Unlike ROS, Daylite is schemaless by design. Any well-formed data may be published
to a topic. It is up to the recipient to discern a message's contents.

## Future work

In the future, Daylite will support automatically extending its node graph through network meshing.
This will allow robot controllers to seamlessly communicate.
