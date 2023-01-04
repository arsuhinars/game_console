#pragma once

#include "game.hpp"
#include "menu.hpp"
#include "utils.hpp"

// Параметры игры
#define PONG_MAX_SCORE        5     // Максимальный счет
#define PONG_IDLE_TIME        1000  // Время задержки в начале раунда
#define PONG_UPDATE_STEP      30    // Длительность шага обновления игры
#define PONG_SUBPIXELS_COUNT  4     // Количество субпикселей в одном пикселе
#define BALL_MAX_SPEED        200   // Максимальная скорость мячика
#define RACKET_ACCEL          600   // Ускорение ракетки
#define RACKET_DRAG           2     // Коэффициент сопротивления движению ракетки
#define RACKET_THICKNESS      3     // Толщина ракетки
#define RACKET_LENGTH         18    // Длина ракетки

class Pong : public Game {
public:
  explicit Pong();

  bool update() override;

private:
  // Перечисление состояний игры
  enum class GameState : uint8_t {
    MAIN_MENU,    // Главное меню
    IDLE,         // Состояние ожидания между раундами
    IDLE_REDRAW,  // То же самое, что и пред., требующее перерисовки всего поля
    ROUND         // Запущенный раунд игры
  };

  // Типы игры
  enum class GameType : uint8_t {
    ONE_PLAYER,   // Игра за одного игрока
    TWO_PLAYERS,  // Игра вдвоем
  };

  Menu _menu;
  
  // Структура, хранящая данные игры
  // Для оптимизации памяти используются битовые поля
  struct {
    GameState state : 2;
    GameType type : 1;
    uint8_t left_score : 4;
    uint8_t right_score : 4;
  } _game_data = { GameState::MAIN_MENU, GameType::ONE_PLAYER };

  // Строка с текстом счета
  char _score_text[6];

  // Позиции и скорости каждой ракетки
  int16_t _left_racket_pos = 0;
  int16_t _left_racket_vel = 0;
  int16_t _right_racket_pos = 0;
  int16_t _right_racket_vel = 0;

  // Позиция и скорость мячика
  utils::ivec2 _ball_pos = { 0, 0 };
  utils::ivec2 _ball_vel = { 0, 0 };

  unsigned long _last_time = 0;

  /// Метод запуска нового раунда
  void startNewRound();
};
