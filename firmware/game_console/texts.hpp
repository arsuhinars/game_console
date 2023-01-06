#pragma once

#include <avr/pgmspace.h>

/// Файл со всеми строками текста, хранящимися в Flash памяти
namespace texts {
  // Названия игр
  extern const char SNAKE_NAME[] PROGMEM;
  extern const char PONG_NAME[] PROGMEM;
  extern const char DINO_NAME[] PROGMEM;
  extern const char TETRIS_NAME[] PROGMEM;

  // Тексты для игровых меню
  extern const char START[] PROGMEM;
  extern const char ONE_PLAYER[] PROGMEM;
  extern const char TWO_PLAYERS[] PROGMEM;
  extern const char QUIT[] PROGMEM;

  // Текст для игры Pong
  extern const char YOU_WIN[] PROGMEM;
  extern const char YOU_LOSE[] PROGMEM;
  extern const char LEFT_WIN[] PROGMEM;
  extern const char RIGHT_WIN[] PROGMEM;
  extern const char DRAW[] PROGMEM;

  // Текст для игры Dino
  extern const char START_HINT[] PROGMEM;
  extern const char EXIT_HINT[] PROGMEM;
} // namespace texts
