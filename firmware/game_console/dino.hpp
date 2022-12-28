#pragma once

#include <avr/pgmspace.h>

#include "game.hpp"

// Храним название игры в Flash памяти
const char DINO_NAME[] PROGMEM = "Dino";

class Dino : public Game {
public:
  explicit Dino();

  void update() override;
};
