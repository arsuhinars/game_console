#include "utils.hpp"

size_t utils::rus_strlen(const char *s) {
  size_t i = 0;
  size_t length = 0;
  while (true) {
    uint8_t c = (uint8_t)s[i];

    if (c == '\0') {
      break;
    }

    if (c <= 191 && (i == 0 || c != 128 || (uint8_t)s[i - 1] != 226)) {
      ++length;
    }
    ++i;
  }
  return length;
}

size_t utils::rus_strlen_P(PGM_P s) {
  size_t i = 0;
  size_t length = 0;
  while (true) {
    uint8_t c = pgm_read_byte(&s[i]);

    if (c == '\0') {
      break;
    }

    if (c <= 191 && (i == 0 || c != 128 || pgm_read_byte(&s[i - 1]) != 226)) {
      ++length;
    }
    ++i;
  }
  return length;
}
