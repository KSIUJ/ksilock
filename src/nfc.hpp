#pragma once
#include <pn532.h>
#include <array>

namespace nfc {
  using uid = std::array<uint8_t, 7>;
  void setup(PN532& rfid);
  void read_nfc_tag(PN532& rfid, uid& uid, uint8_t& uid_lenght);
  bool is_rfid_tag_null(uid& uid);
}
