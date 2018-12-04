#pragma once

namespace util {
template<class T>
void light_up(T& led) {
  led = 1;
}

template<class T, class... Types>
void light_up(T& head, Types... tail) {
  light_up(head);
  light_up(tail...);
}
}
