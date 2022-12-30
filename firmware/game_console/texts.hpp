#pragma once

/// Файл со всеми строками текста, хранящимися в Flash памяти
namespace texts {
  // Названия игр
  const char SNAKE_NAME[] PROGMEM = "Змейка";
  const char PONG_NAME[] PROGMEM = "Pong";
  const char DINO_NAME[] PROGMEM = "Dino";
  const char TETRIS_NAME[] PROGMEM = "Тетрис";

  // Тексты для игровых меню
  const char ONE_PLAYER[] PROGMEM = "1 игрок";
  const char TWO_PLAYERS[] PROGMEM = "2 игрока";
  const char QUIT[] PROGMEM = "Выйти";

  // Текст для игры Pong
  const char YOU_WIN[] PROGMEM = "Вы выиграли";
  const char YOU_LOSE[] PROGMEM = "Вы проиграли";
  const char LEFT_WIN[] PROGMEM = "1 игрок выиграл";
  const char RIGHT_WIN[] PROGMEM = "2 игрок выиграл";
  const char DRAW[] PROGMEM = "Ничья";
} // namespace texts
