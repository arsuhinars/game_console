#include "utils.hpp"

size_t utils::rus_strlen(const char *s) {
  size_t i = 0;
  size_t length = 0;
  while (s[i]) {
    if ((s[i] & 0xC0) != 0x80) length++;
    ++i;
  }
  return length;
}

size_t utils::rus_strlen_P(PGM_P s) {
  size_t i = 0;
  size_t length = 0;
  while (pgm_read_byte(&s[i])) {
    if ((pgm_read_byte(&s[i]) & 0xc0) != 0x80) length++;
    ++i;
  }
  return length;
}
