#pragma once

#include "game.hpp"

#include <avr/pgmspace.h>

// Массив, содержащий все игры
extern const GameBuilder games[];

// Массив с названиями игр
extern const char* const games_names[];

// Количество игр
extern const uint8_t games_count;
