#include <mbed.h>
#include <UIPEthernet.h>
#include <PN532.h>
#include <PN532_SPI.h>

using namespace std;

// #define DEBUG

// Networking
UIPEthernetClass UIPEthernet(PA_7, PA_6, PA_5, PB_6); //mosi, miso, sck, cs
const uint8_t MAC[6] = { 0x00, 0x80, 0x48, 0xba, 0xd1, 0x30 };
const IPAddress IP(192,168,0,108);
const IPAddress DNS(149,156,65,10);
const IPAddress GATEWAY(192,168,0,1);
const IPAddress SUBNET(255,255,255,0);

const IPAddress REMOTE_IP(149,156,65,221);
const uint16_t REMOTE_PORT = 80;

// NFC
SPI nfc_spi(PB_15, PB_14, PB_13); //mosi, miso, sclk
PN532_SPI nfc_interface(nfc_spi, PB_2); //sc
PN532 nfc(nfc_interface);

// Status leds
DigitalOut led_green(PC_5, 0);
DigitalOut led_red(PC_6, 0);

// Lock signal
DigitalOut lock_signal(PC_8, 0);

#ifdef DEBUG
Serial serial(SERIAL_TX, SERIAL_RX);
#endif

void setupNFC() {
  nfc.begin();
  uint32_t nfc_firmware = nfc.getFirmwareVersion();
  if (!nfc_firmware) {
    #ifdef DEBUG
    serial.printf("Didn't find PN53x board, locking up.");
    #endif
    while (true) {
    }
  }
  #ifdef DEBUG
  serial.printf("Found chip PN5%x, ",
                (nfc_firmware >> 24) & 0xFF); 
  serial.printf("firmware version %d.%d.\n",
                (nfc_firmware >> 16) & 0xFF,
                (nfc_firmware >> 8) & 0xFF);
  #endif
  nfc.SAMConfig();
}

void setupEthernet() {
  UIPEthernet.begin(MAC, IP, DNS, GATEWAY, SUBNET);
  #ifdef DEBUG
  serial.printf("%d.%d.%d.%d\n",
                UIPEthernet.localIP()[0],
                UIPEthernet.localIP()[1],
                UIPEthernet.localIP()[2],
                UIPEthernet.localIP()[3]);
  serial.printf("%d.%d.%d.%d\n",
                UIPEthernet.subnetMask()[0],
                UIPEthernet.subnetMask()[1],
                UIPEthernet.subnetMask()[2],
                UIPEthernet.subnetMask()[3]);
  serial.printf("%d.%d.%d.%d\n",
                UIPEthernet.gatewayIP()[0],
                UIPEthernet.gatewayIP()[1],
                UIPEthernet.gatewayIP()[2],
                UIPEthernet.gatewayIP()[3]);
  serial.printf("%d.%d.%d.%d\n",
                UIPEthernet.dnsServerIP()[0],
                UIPEthernet.dnsServerIP()[1],
                UIPEthernet.dnsServerIP()[2],
                UIPEthernet.dnsServerIP()[3]);
  #endif
}

void readNFCTag(uint8_t* uid) {
  uint8_t uidLen = 0;
  uint16_t timeout = 1000;
  bool nfc_res = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, timeout);
  #ifdef DEBUG
  serial.printf("Card id: %02x%02x%02x%02x\n", uid[0], uid[1], uid[2], uid[3]);
  #endif
}

bool isNFCTagNull(uint8_t* uid) {
  if (uid[0] == 0 && uid[1] == 0 && uid[2] == 0 && uid[3] == 0)
    return true;
  return false;
}

bool authorizeNFCTag(uint8_t* uid) {
  #ifdef DEBUG
  serial.printf("Authorizing tag...\n");
  #endif
  UIPClient client;
  client.connect(REMOTE_IP, REMOTE_PORT);
  if (!client.connected())
    return false;
  #ifdef DEBUG
  serial.printf("Connected to remote server.\n");
  #endif
  uint8_t* buff = (uint8_t*) calloc(200, sizeof(uint8_t));
  const char* format = "GET http://149.156.65.221/authorize.json?card_id=%02x%02x%02x%02x HTTP/1.0\n\n";
  sprintf((char *) buff,
          format,
          uid[0],
          uid[1],
          uid[2],
          uid[3]);
  client.write(buff, 200);
  free(buff);

  uint32_t bytes = 0;
  #ifdef DEBUG
  uint32_t retries = 0;
  #endif
  while(!(bytes = client.available())) {
    wait_ms(100);
    #ifdef DEBUG
    serial.printf("Czekam na pakiet %d\n", retries++);
    #endif
  }
  bool flag = false;
  buff = (uint8_t*) calloc(bytes, sizeof(uint8_t));
  do {
    client.read(buff, bytes);
    #ifdef DEBUG
    serial.printf("%s", buff);
    #endif  
    if(buff[9] == '2' && buff[10] == '0' && buff[11] == '0') {
      flag = true;
    }
  } while ((bytes = client.available()));
  free(buff);
  return flag;
}

void unlock() {
  lock_signal = 1;
  led_red = 0;
  wait_ms(3000);
  lock_signal = 0;
  led_green = 0;
}

int main() {
  int32_t cycles_till_restart = 2 * 60 * 60 * 6;
  setupNFC();
  setupEthernet();
  #ifdef DEBUG
  serial.printf("Initialization done.");
  #endif
  while (true) {
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    readNFCTag(uid);
    if (!isNFCTagNull(uid)) {
      led_green = 1;
      led_red = 1;
      if (authorizeNFCTag(uid)) {
        unlock();
      }
      else {
        led_green = 0;
        wait_ms(1000);
        led_red = 0;
      }
    }
    wait_ms(500);
    
    if (--cycles_till_restart < 0) {
      led_green = 1;
      wait_ms(300);
      led_green = 0;
      NVIC_SystemReset();
    }
  }
  return 0;
}
