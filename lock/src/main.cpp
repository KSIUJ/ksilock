#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <UIPEthernet.h>

// SPI
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
byte mac[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB };
IPAddress ip(192,168,88,37);
IPAddress server(192,168,88,4);
EthernetUDP udp;

void setup() {
  Serial.begin(9600);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.println("Did not find the shield - locking up");
    while (true) {
    }
  }

  Serial.print("Found chip PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); 
  Serial.println((versiondata>>8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();

  nfc.begin();

  Ethernet.begin(mac, ip);
  Serial.print("Lock ip: ");
  Serial.println(Ethernet.localIP());
}

void loop(void) {
  int success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength;  // 4 - classic mifare - expected for ELS
    
  // wait for card and get data from it
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: "); nfc.PrintHex(uid, uidLength); Serial.println("");
  
    int len = 0;
    do {
      success = udp.beginPacket(IPAddress(192,168,88,4),7777);
    } while (!success);
  
    for(int i = 0; i < 4; ++i) {
      int older = uid[i] >> 4 & 0x0F + '0';
      int younger = uid[i] & 0x0F + '0';
      udp.write((int)'A');
      udp.write((int)'B');
    }
    udp.endPacket();
  }
  delay(1000);
}