#pragma once

#include <avr/pgmspace.h>

#include "game.hpp"

class Snake : public Game {
public:
  explicit Snake();

  bool update() override;
};
