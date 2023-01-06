#pragma once

#include "game.hpp"
#include "utils.hpp"

// Макс. кол-во птеродактелей на экране
#define DINO_MAX_BIRDS      1
// Макс. кол-во кактусов на экране
#define DINO_MAX_CACTI      1
// Время одного шага обновления игры
#define DINO_UPDATE_STEP    30

// Смещение заголовка игры от верхнего края
#define DINO_TITLE_MARGIN   8
// Высота земли на экране
#define DINO_GROUND_HEIGHT  8
// Положение динозаврика по горизонтали
#define DINO_POS_X          8
// Отступ текста со счетом от левого края
#define DINO_SCORE_MARGIN   8

// Базовая скорость движения
#define DINO_BASE_SPEED       50
// Ускорение свободного падения динозаврика
#define DINO_GRAVITY          250
// Ускорение при приседании динозаврика
#define DINO_DUCK_GRAVITY     700
// Скорость при прыжке
#define DINO_JUMP_SPEED       120
// Скорость при высоком прыжке
#define DINO_HIGH_JUMP_SPEED  150
// Кол-во очков для смены спрайта анимации
#define DINO_ANIM_STEP        1
// Базовая скорость получения очков
#define DINO_BASE_SCORE_RATE  10
// Мин. кол-во очков для спавна препятствия
#define DINO_MIN_OBSTACLE_RATE  5
// Макс. кол-во очков для спавна препятствия
#define DINO_MAX_OBSTACLE_RATE  20
// Вероятность спавна птицы. 1/N
#define DINO_BIRD_SPAWN_RATE    4
// Максимальная высота спавна птиц от нижнего края экрана
#define DINO_BIRD_SPAWN_HEIGHT  58
// Скорость движения птиц
#define DINO_BIRD_MOVE_SPEED    25
// Кол-во очков для смены спрайта птицы
#define DINO_BIRD_ANIM_STEP     2

class Dino : public Game {
public:
  explicit Dino();

  bool update() override;

private:
  struct {
    bool is_active;           // Активна ли птица
    int16_t old_pos_x;        // Позиция птицы в предыдущем кадре
    utils::ivec2 pos;         // Позиция птицы на экране
  } _birds[DINO_MAX_BIRDS];   // Пул всех птиц на экране

  struct {
    bool is_active;           // Активен ли кактус
    int16_t old_pos_x;        // Позиция кактуса в предыдущем кадре
    int16_t pos_x;            // Позиция кактуса по оси X
    uint8_t type;             // Тип кактуса. 0 - маленький, 1 - большой
  } _cacti[DINO_MAX_CACTI];   // Пул всех кактусов на экране

  bool _is_menu_rendered;     // Было ли уже отрисовано меню
  bool _is_game_started;      // Запущена ли игра
  bool _is_game_lost = false; // Проиграл ли игрок
  unsigned long _last_time;   // Игровой таймер

  // Кол-во очков для спавна следующего препятствия
  uint16_t _next_obstacle_val;

  int16_t _old_dino_y;        // Позиция динозаврика в прошлом кадре
  int16_t _dino_y;            // Координата динозаврика по Y
  int16_t _dino_vel;          // Скорость динозаврика по Y

  uint8_t _frame_counter;     // Счетчик кадров
  uint16_t _game_counter;     // Счетчик игры

  /// Метод отрисовки игры
  void renderGame();

  /// Метод сброса состояния игры
  void resetGame();
  
  /// Метод спавна кактусов
  void spawnCacti();

  /// Метод спавна птицы
  void spawnBird();
};
