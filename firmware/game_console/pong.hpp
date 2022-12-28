#pragma once

#include <avr/pgmspace.h>

#include "game.hpp"

// Храним название игры в Flash памяти
const char PONG_NAME[] PROGMEM = "Pong";

class Pong : public Game {
public:
  explicit Pong();

  void update() override;
};
