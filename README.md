# KSI Lock

This project focuses on creating solution for managing access to KSI places.
It consists of software for stm nucleo, responsible for communicating with
RFID reader and ethernet module and with external auth server - currently
we use KSI ERC, a system written to keep track of members list.

Our student ID cards will be used as keys to the lock. They are equipped
with 13.56MHz RFID tag, Classic MiFare.

Software is written in C++ using platformio.org as a library manager. Libraries used:
*   [UIPEthernet](http://platformio.org/#!/lib/show/91/UIPEthernet)
*   [Adafruit-PN532](http://platformio.org/#!/lib/show/29/Adafruit-PN532)

# Authors

*   Kamil Drobniak - he has done all the soldering, drilling, hammering and so on - 
    we installed all hardware by ourselves.
*   Adam Piekarczyk - here I am, I've helped Kamil and also I'm responsible for the software.
