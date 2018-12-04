#pragma once
#include <pn532.h>

namespace nfc {
  void setup(PN532& rfid);
  void read_nfc_tag(PN532& rfid, uint8_t (&uid)[7], uint8_t& uid_lenght);
  bool is_rfid_tag_null(uint8_t (&uid)[7]);
}
