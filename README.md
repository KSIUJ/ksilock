# KSI Lock

This project focuses on creating solution for managing access to KSI places.
It is splitted int two parts:
*   lock - software for arduino, responsible for communicating with RFID reader and ethernet module
*   gatekeeper - system daemon, responsible for receiving messages from lock and for authentication and authorization

Our student ID cards will be used as keys to the lock. They are equipped
with 13.56MHz RFID tag, Classic MiFare.

## Lock

Written in C++ using platformio.org as a library manager. Libraries used:
*   [UIPEthernet](http://platformio.org/#!/lib/show/91/UIPEthernet)
*   [Adafruit-PN532](http://platformio.org/#!/lib/show/29/Adafruit-PN532)

As for development we use Arduino Mega 2560; in production we want to use
something smaller.

## Gatekeeper

Written in Rust. It is supposed to run on nightly build until KSI Lock
official release when Rust version will be fixed.

### Protocol

Messages are transferred using UDP. Each datagram is 128 bytes long and starts with single `u8` integer - packet type.

#### Packet types
`1` - AUTH packet, containing RFID card ID - 4 bytes long.
