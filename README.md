= KSI Lock =

This project focuses on creating solution for managing access to KSI places.
It is splitted int two parts:
*   lock - software for arduino, responsible for communicating with RFID reader and ethernet module
*   gatekeeper - system daemon, responsible for receiving messages from lock and for authentication and authorization

== Lock ==

== Gatekeeper ==

=== Protocol ===

Messages are transferred using UDP. Each datagram is 128 bytes long and starts with single `u8` integer - packet type.

==== Packet types ====
`1` - AUTH packet, containing RFID card ID - 4 bytes long.
