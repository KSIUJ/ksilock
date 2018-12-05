#include <array>
#include <utility>
#include <UIPEthernet.h>
#include "nfc.hpp"
#include "util.hpp"

namespace net {
  using mac = std::array<uint8_t, 6>;
  using server_address = std::pair<IPAddress, uint16_t>;
  void initialize(UIPEthernet& ethernet, mac& mac);
  bool authorize(UIPEthernet& ethernet, server_address& addr, nfc::uid& uid);
}
