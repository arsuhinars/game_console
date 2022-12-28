#pragma once

#include <avr/pgmspace.h>

namespace utils {
  /// Функция вычисления длины строки, содержащей русские символы в кодировке UTF-8
  size_t rus_strlen(const char* s);

  /// Аналогично функции `rus_strlen` вычисляет длину строки,
  /// однако принимает PROGMEM строку
  size_t rus_strlen_P(PGM_P s);

} // namespace utils
