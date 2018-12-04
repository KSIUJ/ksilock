#include <mbed.h>
#include <UIPEthernet.h>
#include <pn532.h>
#include "nfc.hpp"
#include "util.hpp"

#define DEBUG

/* // Networking */
/* UIPEthernetClass UIPEthernet(PA_7, PA_6, PA_5, PB_6); //mosi, miso, sck, cs */
/* const uint8_t MAC[6] = { 0x00, 0x80, 0x48, 0xba, 0xd1, 0x30 }; */
/* const IPAddress IP        (192,168,  0,108); */
/* const IPAddress DNS       (149,156, 65, 10); */
/* const IPAddress GATEWAY   (192,168,  0,  1); */
/* const IPAddress SUBNET    (255,255,255,  0); */
/* const IPAddress REMOTE_IP (149,156, 65,221); */
/* const uint16_t REMOTE_PORT = 80; */
/*  */
// NFC
PN532 rfid(PB_15, PB_14, PB_13, PB_2); // mosi, miso, sclk, sc

// Status leds
DigitalOut led_green(PC_5, 0);
DigitalOut led_red(PC_6, 0);

// Lock signal
DigitalOut lock_signal(PC_8, 0);

// Serial
#ifdef DEBUG
Serial serial(SERIAL_TX, SERIAL_RX);
#endif

int main() {
  nfc::setup(rfid);
/*   setupEthernet(); */
  #ifdef DEBUG
  serial.printf("Initialization done.");
  #endif
  while (true) {
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uid_lenght = 0;
    nfc::read_nfc_tag(rfid, uid, uid_lenght);
    if (!nfc::is_rfid_tag_null(uid)) {
      util::light_up(led_green, led_red);
/*       if ((uid[0] == 0xDC && uid[1] == 0xAF && uid[2] == 0x07 && uid[3] == 0x09) || */
/*           (uid[0] == 0x24 && uid[1] == 0x87 && uid[2] == 0x4B && uid[3] == 0x1A)) { */
/*         unlock(); */
/*       } else if (authorizeNFCTag(uid)) { */
/*         unlock();  */
/*       } else { */
/*         refuse();  */
/*       } */
    }
    wait_ms(500);
    util::attempt_device_reset([&](){util::system_reset_signal(led_green, led_red);});
  }
  return 0;
}
