#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <UIPEthernet.h>
#include "aes256.h"
#include "aes256_key.h"

// SPI
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// NFC
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Ethernet communication
byte mac[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB };
IPAddress ip(192,168,88,37);
IPAddress server(192,168,88,4);
EthernetUDP udp;

// AES
aes256_context aes_context;
extern uint8_t key[32];

void setup_nfc() {
  Serial.println("Initializing NFC...");
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Did not find the shield - locking up");
    while (true) {}
  }

  Serial.print("Found chip PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); 
  Serial.println((versiondata>>8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();
}

void setup_ethernet() {
  Serial.println("Initializing ethernet...");
  Ethernet.begin(mac, ip);
  Serial.print("Lock ip: ");
  Serial.println(Ethernet.localIP());
}

void setup_aes() {
  Serial.print("Initializing AES256... ");
  aes256_init(&aes_context, key);
  Serial.println("OK!");
}

void setup() {
  Serial.begin(9600);
  setup_nfc();
  setup_ethernet();
  setup_aes();
}

const static uint8_t AUTH = 1;

void prepare_auth_msg(const uint8_t *uid, uint8_t *buffer) {
  buffer[0] = AUTH;
  for(int32_t i = 0; i < 4; ++i) {
    buffer[1 + i] = uid[i];
  }
  aes256_encrypt_ecb(&aes_context, buffer);
} 

void send_auth_msg(const uint8_t *buffer) {
  int32_t success = -1;
  do {
    success = udp.beginPacket(IPAddress(192,168,88,4),7777);
  } while (!success);
  for(int32_t i = 0; i < 128; ++i) {
    udp.write(buffer[i]);
  }
  int32_t res = udp.endPacket();

  #ifdef DEBUG
  if(res == 1) {
    Serial.println("Packet sent.");
  } else {
    Serial.println("Packet could not be sent.");
  }
  #endif
}

void loop(void) {
  int32_t success = -1;
  uint8_t buffer[128] = { 0 };
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength = 0;  // 4 - classic mifare - expected for ELS
    
  // wait for card and get data from it
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    #ifdef DEBUG
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: "); nfc.PrintHex(uid, uidLength); Serial.println("");
    #endif

    prepare_auth_msg(uid, buffer);
    send_auth_msg(buffer);
  }
  delay(1000);
}