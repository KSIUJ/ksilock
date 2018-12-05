#include "nfc.hpp"

namespace nfc {

void setup(PN532& rfid) {
  // rfid.begin();
  uint32_t firmware_version = rfid.getFirmwareVersion();
  if (!firmware_version) {
    #ifdef DEBUG
    serial.printf("Didn't find PN53x board, locking up.");
    #endif
    while (true) {}
  }
  #ifdef DEBUG
  serial.printf("Found chip PN5%x, ",
                (nfc_firmware >> 24) & 0xFF); 
  serial.printf("firmware version %d.%d.\n",
                (nfc_firmware >> 16) & 0xFF,
                (nfc_firmware >> 8) & 0xFF);
  #endif
  rfid.SAMConfig();
}

void read_nfc_tag(PN532& rfid, uid& uid, uint8_t& uid_length) {
  uint16_t timeout = 1000;
  rfid.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid.data(), &uid_length, timeout);
  #ifdef DEBUG
  if (uid_length == 4) {
    serial.printf("Found Mifare Classic card.");
  } else if (uid_length == 7) {
    serial.printf("Found Mifare Ultralight card.");
  } else {
    serial.printf("Found unknown card.");
  }
  // TODO different card types
  serial.printf("Card id: %02x%02x%02x%02x\n", uid[0], uid[1], uid[2], uid[3]);
  #endif
}

bool is_rfid_tag_null(uid& uid) {
  if (uid[0] == 0 && uid[1] == 0 && uid[2] == 0 && uid[3] == 0 && uid[4] == 0 && uid[5] == 0 && uid[6] == 0)
    return true;
  return false;
}
}

/* bool authorizeNFCTag(uint8_t* uid) { */
/* #ifdef DEBUG */
/*   serial.printf("Authorizing tag...\n"); */
/* #endif */
/*   UIPClient client; */
/*   client.connect(REMOTE_IP, REMOTE_PORT); */
/*   if (!client.connected()) */
/*     return false; */
/* #ifdef DEBUG */
/*   serial.printf("Connected to remote server.\n"); */
/* #endif */
/*   uint8_t* buff = (uint8_t*) calloc(200, sizeof(uint8_t)); */
/*   const char* format = "GET http://149.156.65.221/authorize.json" */
/*                        "?card_id=%02x%02x%02x%02x HTTP/1.0\n\n"; */
/*   sprintf((char *) buff, */
/*           format, */
/*           uid[0], */
/*           uid[1], */
/*           uid[2], */
/*           uid[3]); */
/*   client.write(buff, 200); */
/*   free(buff); */
/*  */
/*   uint32_t bytes = 0; */
/* #ifdef DEBUG */
/*   uint32_t retries = 0; */
/* #endif */
/*   while(!(bytes = client.available())) { */
/*     wait_ms(100); */
/* #ifdef DEBUG */
/*     serial.printf("Czekam na pakiet %d\n", retries++); */
/* #endif */
/*   } */
/*   bool flag = false; */
/*   buff = (uint8_t*) calloc(bytes, sizeof(uint8_t)); */
/*   do { */
/*     client.read(buff, bytes); */
/* #ifdef DEBUG */
/*     serial.printf("%s", buff); */
/* #endif   */
/*     if(buff[9] == '2' && buff[10] == '0' && buff[11] == '0') { */
/*       flag = true; */
/*     } */
/*   } while ((bytes = client.available())); */
/*   free(buff); */
/*   return flag; */
/* } */
/*  */
/*  */
