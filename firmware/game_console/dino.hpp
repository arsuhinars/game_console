#pragma once

#include "game.hpp"
#include "utils.hpp"

// Макс. кол-во птеродактелей на экране
#define DINO_MAX_BIRDS      2
// Макс. кол-во кактусов на экране
#define DINO_MAX_CACTI      6
// Время одного шага обновления игры
#define DINO_UPDATE_STEP    30

// Высота земли на экране
#define DINO_GROUND_HEIGHT  8
// Положение динозаврика по горизонтали
#define DINO_POS_X          8
// Отступ текста со счетом от левого края
#define DINO_SCORE_PADDING  8

// Ускорение свободного падения динозаврика
#define DINO_GRAVITY          250
// Скорость при прыжке
#define DINO_JUMP_SPEED       95
// Скорость при высоком прыжке
#define DINO_HIGH_JUMP_SPEED  115
// Кол-во кадров для смены спрайта анимации
#define DINO_ANIM_STEP        16
// Базовая скорость получения очков
#define DINO_BASE_SCORE_RATE  16

class Dino : public Game {
public:
  explicit Dino();

  bool update() override;

private:
  struct {
    bool is_active; // Активна ли птица
    utils::cvec2 pos;      // Позиция птицы на экране
  } _birds[DINO_MAX_BIRDS];   // Пул всех птиц на экране

  struct {
    bool is_active; // Активен ли кактус
    int8_t pos_x;   // Позиция кактуса по оси X
  } _cacti[DINO_MAX_CACTI];   // Пул всех кактусов на экране

  bool _is_game_started;      // Запущена ли игра
  unsigned long _last_time;   // Игровой таймер

  int8_t _dino_y;         // Координата динозаврика по Y
  int8_t _dino_vel;       // Скорость динозаврика по Y

  uint8_t _frame_count;   // Счетчик кадров
  uint16_t _score;        // Счет игры

  /// Метод запуска игры
  void startGame();
};
