#include "nfc.hpp"

namespace nfc {

void initialize(PN532& rfid) {
  // rfid.begin();
  uint32_t firmware_version = rfid.getFirmwareVersion();
  if (!firmware_version) {
    #ifndef NDEBUG
    serial.printf("Didn't find PN53x board, locking up.");
    #endif
    while (true) {}
  }
  #ifndef NDEBUG
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
  #ifndef NDEBUG
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


