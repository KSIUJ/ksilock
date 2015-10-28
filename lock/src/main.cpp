#include <mbed.h>
#include <UIPEthernet.h>
#include <PN532.h>
#include <PN532_SPI.h>

using namespace std;


// Networking
UIPEthernetClass UIPEthernet(PA_11, PA_12, PA_13, PA_8); //miso, mosi, sck, cs
const uint8_t MAC[6] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAD };
const IPAddress IP(192,168,1,175);
const uint16_t PORT = 80;

const IPAddress REMOTE_IP(192,168,1,1);
const uint16_t REMOTE_PORT = 80;

// NFC
SPI nfc_spi(PA_5, PA_6, PA_7); //mosi, miso, sclk
PN532_SPI nfc_interface(nfc_spi, PA_9);
PN532 nfc(nfc_interface); // cs

// Status leds
DigitalOut led_green(PA_1);
DigitalOut led_red(PA_2);

void setup() {
  UIPEthernet.begin(MAC, IP);
  nfc.begin();
  nfc.SAMConfig();
}

#ifdef DEBUG
Serial serial(USBTX, USBRX);
#endif

int main() {
  uint32_t nfc_firmware = nfc.getFirmwareVersion();
  #ifdef DEBUG
  serial.printf("Firmware version: %d\n", nfc_firmware);
  #endif
  
  while (true) {
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLen = 0;
    uint16_t timeout = 1000;
    bool res = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, timeout);
    #ifdef DEBUG
    serial.printf("Card id: %02x%02x%02x%02x\n", uid[0], uid[1], uid[2], uid[3]);
    #endif

    // UIPClient client;
    // int res = 0;
    // while (!(res = client.connect(REMOTE_IP, REMOTE_PORT)) {
      
    // }
  }

  return 0;
}