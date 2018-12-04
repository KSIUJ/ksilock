#include <functional>
#include <mbed.h>

namespace util {
  void attempt_device_reset(std::function<void(void)> signal);
  void system_reset_signal(DigitalOut& led_green, DigitalOut& led_red);
  template<class T>
  void light_up(T& led);
  template<class T, class... Types>
  void light_up(T& head, Types... tail);
  void unlock_and_signal(DigitalOut& lock, DigitalOut& led_green, DigitalOut& led_red);
  void refuse_signal(DigitalOut& led_gree, DigitalOut& led_red);
}

#include "util.tpp"
