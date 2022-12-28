#pragma once

#include <avr/pgmspace.h>

#include "game.hpp"

// Храним название игры в Flash памяти
const char SNAKE_NAME[] PROGMEM = "Змейка";

class Snake : public Game {
public:
  explicit Snake();

  void update() override;
};
