#include <functional>
#include <mbed.h>
#include "util.hpp"

namespace util {
// Reset counter - 2 (main loop cycles) * 60 (seconds) * 60 (minutes) * 6 (hours)
int32_t cycles_till_restart = 2 * 60 * 60 * 6;

void unlock_and_signal(DigitalOut& lock, DigitalOut& led_green, DigitalOut& led_red) {
  lock = 1;
  led_red = 0;
  wait_ms(3000);
  lock = 0;
  led_green = 0;
}

void refuse_and_signal(DigitalOut& led_green, DigitalOut& led_red) {
  led_green = 0;
  wait_ms(1000);
  led_red = 0;
}

void system_reset_signal(DigitalOut& led_green, DigitalOut& led_red) {
  for (int i = 0; i < 4; ++i) {
    led_green = 1;
    wait_ms(25);
    led_red = 1;
    wait_ms(25);
    led_green = 0;
    wait_ms(25);
    led_red = 0;
    wait_ms(25); 
  } 
}

void attempt_device_reset(std::function<void(void)> signal) {
  if (--cycles_till_restart < 0) {
    signal();
    NVIC_SystemReset();
  }
}

void format_authorization_request(std::array<uint8_t, 200>& buff, IPAddress& server_ip, nfc::uid& uid) {
  const char* format = "GET http://%s/lock/authorize"
                       "?card_id=%02x%02x%02x%02x HTTP/1.0\n\n"; 
  sprintf(
      reinterpret_cast<char*>(buff.data()),
      format,
      server_ip.toString(),
      uid[0],
      uid[1],
      uid[2],
      uid[3]);
}
}
