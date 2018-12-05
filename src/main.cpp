#include <array>
#include <mbed.h>
#include <UIPEthernet.h>
#include <pn532.h>
#include "nfc.hpp"
#include "util.hpp"
#include "net.hpp"


// Networking
// has to have name as below and be global or else you will get linker errors
UIPEthernet uIPEthernet(PA_7, PA_6, PA_5, PB_6); //mosi, miso, sck, cs
net::mac mac{{0x00, 0x80, 0x48, 0xba, 0xd1, 0x30}};
net::server_address remote_addr{{149, 156, 65, 221}, 80};

// NFC
PN532 rfid(PB_15, PB_14, PB_13, PB_2); // mosi, miso, sclk, sc

// Status leds
DigitalOut led_green(PC_5, 0);
DigitalOut led_red(PC_6, 0);

// Lock signal
DigitalOut lock(PC_8, 0);

// Serial
#ifndef NDEBUG
Serial serial(SERIAL_TX, SERIAL_RX);
#endif

int main() {
  nfc::initialize(rfid);
  net::initialize(uIPEthernet, mac);
  #ifndef NDEBUG
  serial.printf("Initialization done.");
  #endif
  while (true) {
    nfc::uid uid{{0, 0, 0, 0, 0, 0, 0}}; 
    uint8_t uid_lenght = 0;
    nfc::read_nfc_tag(rfid, uid, uid_lenght);
    if (!nfc::is_rfid_tag_null(uid)) {
      util::light_up(led_green, led_red);
      if ((uid[0] == 0xDC && uid[1] == 0xAF && uid[2] == 0x07 && uid[3] == 0x09) ||
          (uid[0] == 0x24 && uid[1] == 0x87 && uid[2] == 0x4B && uid[3] == 0x1A)) {
        util::unlock_and_signal(lock, led_green, led_red);
      } else if (net::authorize(uIPEthernet, remote_addr, uid)) {
        util::unlock_and_signal(lock, led_green, led_red); 
      } else {
        util::refuse_and_signal(led_green, led_red); 
      }
    }
    wait_ms(500);
    util::attempt_device_reset([&](){util::system_reset_signal(led_green, led_red);});
  }
  return 0;
}
