#pragma once

#include <avr/pgmspace.h>

#include "game.hpp"

class Dino : public Game {
public:
  explicit Dino();

  bool update() override;
};
