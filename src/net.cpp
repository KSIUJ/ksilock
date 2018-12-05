#include "net.hpp"

namespace net {
void initialize(UIPEthernet& ethernet, net::mac& mac) {
  #ifndef NDEBUG
  serial.printf("Searching for DHCP server...");
  #endif
  if (ethernet.begin(mac.data()) != 1) {
    #ifndef NDEBUG
    serial.printf("No DHCP server found, locking up.");
    #endif
    while (true) {}
  }
  #ifndef NDEBUG
  serial.printf("IP %s", ethernet.localIP().toString());
  #endif
}

void wait_for_response(EthernetClient& client) {
  uint32_t timeout = time(NULL) + 5;
  while (client.available() == 0) {
    if(time(NULL) > timeout) {
      client.stop();
      #ifndef NDEBUG
      serial.printf("Connection timed out, disconnected.\n");
      #endif
      break;
    }
  }
}

void read_response(EthernetClient& client, std::array<uint8_t, 200>& buff) {
  uint32_t size = 0;
  uint32_t i = 0;
  while ((size = client.available()) > 0) {
    if (i + size >= buff.max_size()) {
      #ifndef NDEBUG
      serial.printf("Too long response, truncating.\n");
      #endif
      break;
    }
    client.read(buff.data() + i, size);
    #ifndef NDEBUG
    serial.printf("%s", reinterpret_cast<char*>(buff.data() + i));
    #endif
  }
}

bool authorize(UIPEthernet& ethernet, server_address& addr, nfc::uid& uid) {
  #ifndef NDEBUG
  serial.printf("Authorizing tag...\n");
  #endif
  EthernetClient client;
  if (client.connect(addr.first, addr.second)) {
    #ifndef NDEBUG
    serial.printf("Connected to server.\n");
    #endif
    std::array<uint8_t, 200> buff{0};
    util::format_authorization_request(buff, addr.first, uid);
    client.write(buff.data(), 200);
    wait_for_response(client);
    if (client.connected()) {
      bool flag = false;
      read_response(client, buff);
      if (buff[9] == '2' && buff[10] == '0' && buff[11] == '0') {
        return true;
      }   
    }
  } else {
    #ifndef NDEBUG
    serial.printf("Failed to connect to server.\n");
    #endif
  }
  return false;
}
}
