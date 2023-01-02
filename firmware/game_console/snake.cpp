#include <Arduino.h>

#include "texts.hpp"
#include "utils.hpp"
#include "controls.hpp"
#include "snake.hpp"

using namespace utils;

// Спрайты частей тела змейки
const uint8_t SNAKE_HORIZONTAL[] PROGMEM = {
	0x42, 0x42, 0x52, 0x52, 0x42, 0x42, 0x4A, 0x4A, 
};
const uint8_t SNAKE_VERTICAL[] PROGMEM = {
	0x00, 0xFF, 0x00, 0x0C, 0xC0, 0x00, 0xFF, 0x00, 
};
const uint8_t SNAKE_UP_RIGHT[] PROGMEM = {
	0x00, 0x07, 0x18, 0x23, 0x20, 0x40, 0x4B, 0x4A, 
};
const uint8_t SNAKE_UP_LEFT[] PROGMEM = {
	0x42, 0x4B, 0x48, 0x20, 0x23, 0x18, 0x07, 0x00, 
};
const uint8_t SNAKE_DOWN_RIGHT[] PROGMEM = {
	0x00, 0xE0, 0x18, 0x04, 0xD4, 0x0A, 0xCA, 0x42, 
};
const uint8_t SNAKE_DOWN_LEFT[] PROGMEM = {
	0x42, 0xD2, 0x12, 0xC4, 0x04, 0x18, 0xE0, 0x00, 
};

// Спрайты головы змейки
const uint8_t SNAKE_HEAD_UP[] PROGMEM = {
	0x38, 0xC4, 0x0A, 0x01, 0x01, 0x0A, 0xC4, 0x38, 
};
const uint8_t SNAKE_HEAD_RIGHT[] PROGMEM = {
	0x42, 0x42, 0x81, 0x81, 0xA5, 0x42, 0x24, 0x18, 
};
const uint8_t SNAKE_HEAD_DOWN[] PROGMEM = {
	0x1C, 0x23, 0x50, 0x80, 0x80, 0x50, 0x23, 0x1C, 
};
const uint8_t SNAKE_HEAD_LEFT[] PROGMEM = {
	0x18, 0x24, 0x42, 0xA5, 0x81, 0x81, 0x42, 0x42, 
};

// Объединяем спрайты головы в один массив для оптимизации поиска
const uint8_t* SNAKE_HEAD[] = {
  SNAKE_HEAD_UP, SNAKE_HEAD_RIGHT, SNAKE_HEAD_DOWN, SNAKE_HEAD_LEFT
};

// Спрайты хвоста
const uint8_t SNAKE_TAIL_UP[] PROGMEM = {
	0x00, 0x07, 0x18, 0x20, 0x20, 0x18, 0x07, 0x00, 
};
const uint8_t SNAKE_TAIL_RIGHT[] PROGMEM = {
	0x00, 0x00, 0x18, 0x24, 0x24, 0x42, 0x42, 0x42, 
};
const uint8_t SNAKE_TAIL_DOWN[] PROGMEM = {
	0x00, 0xE0, 0x18, 0x04, 0x04, 0x18, 0xE0, 0x00, 
};
const uint8_t SNAKE_TAIL_LEFT[] PROGMEM = {
	0x42, 0x42, 0x42, 0x24, 0x24, 0x18, 0x00, 0x00, 
};
const uint8_t* SNAKE_TAIL[] = {
  SNAKE_TAIL_UP, SNAKE_TAIL_RIGHT, SNAKE_TAIL_DOWN, SNAKE_TAIL_LEFT
};

const __FlashStringHelper* SNAKE_MENU_ITEMS[] = {
  FPSTR(texts::START), FPSTR(texts::QUIT)
};

Snake::Snake() : _menu(SNAKE_MENU_ITEMS, 2, FPSTR(texts::SNAKE_NAME)) {

}

bool Snake::update() {
  // Если игра не запущена, то рисуем меню
  if (!_is_game_started) {
    _menu.update();

    if (_menu.clickedItem() != -1) {
      if (_menu.clickedItem() == 0) {
        startGame();
        return true;
      } else {
        return false;
      }
    }

    return true;
  }

  // Обрабатываем ввод
  if (snake_dir != SNAKE_DIR_DOWN && controls::up_button.press()) {
    snake_dir = SNAKE_DIR_UP;
  }
  else if (snake_dir != SNAKE_DIR_LEFT && controls::right_button.press()) {
    snake_dir = SNAKE_DIR_RIGHT;
  }
  else if (snake_dir != SNAKE_DIR_UP && controls::down_button.press()) {
    snake_dir = SNAKE_DIR_DOWN;
  }
  else if (snake_dir != SNAKE_DIR_RIGHT && controls::left_button.press()) {
    snake_dir = SNAKE_DIR_LEFT;
  }

  // Обновляем игру только тогда, когда это необходимо
  if (millis() - _last_update <= SNAKE_UPDATE_STEP) {
    return true;
  }
  _last_update = millis();
    
  // Ищем подходящий спрайт
  uint8_t* sprite;
  if (last_snake_dir == snake_dir) {
    sprite = (snake_dir % 2 == 0) ? SNAKE_VERTICAL : SNAKE_HORIZONTAL;
  } else if (
    (last_snake_dir == SNAKE_DIR_UP && snake_dir == SNAKE_DIR_RIGHT) ||
    (last_snake_dir == SNAKE_DIR_LEFT && snake_dir == SNAKE_DIR_DOWN)
  ) {
    sprite = SNAKE_DOWN_RIGHT;
  } else if (
    (last_snake_dir == SNAKE_DIR_UP && snake_dir == SNAKE_DIR_LEFT) ||
    (last_snake_dir == SNAKE_DIR_RIGHT && snake_dir == SNAKE_DIR_DOWN)
  ) {
    sprite = SNAKE_DOWN_LEFT;
  } else if (
    (last_snake_dir == SNAKE_DIR_DOWN && snake_dir == SNAKE_DIR_RIGHT) ||
    (last_snake_dir == SNAKE_DIR_LEFT && snake_dir == SNAKE_DIR_UP)
  ) {
    sprite = SNAKE_UP_RIGHT;
  } else if (
    (last_snake_dir == SNAKE_DIR_DOWN && snake_dir == SNAKE_DIR_LEFT) ||
    (last_snake_dir == SNAKE_DIR_RIGHT && snake_dir == SNAKE_DIR_UP)
  ) {
    sprite = SNAKE_UP_LEFT;
  }

  // Рисуем этот спрайт
  display::oled.drawBitmap(
    snake_head_pos.x * SNAKE_TILE_SIZE, snake_head_pos.y * SNAKE_TILE_SIZE,
    sprite,
    SNAKE_TILE_SIZE, SNAKE_TILE_SIZE,
    0, BUF_REPLACE
  );

  // Обновляем предыдущий тайл
  field[snake_head_pos.x][snake_head_pos.y] = snake_dir;
  last_snake_dir = snake_dir;

  // Двигаем голову по направлению
  auto dir = getDirVector(snake_dir);
  snake_head_pos.x = (snake_head_pos.x + dir.x + SNAKE_FIELD_WIDTH) % SNAKE_FIELD_WIDTH;
  snake_head_pos.y = (snake_head_pos.y + dir.y + SNAKE_FIELD_HEIGHT) % SNAKE_FIELD_HEIGHT;
  // Обновляем текущий тайл головы
  field[snake_head_pos.x][snake_head_pos.y] = snake_dir;

  // Рисуем спрайт головы
  display::oled.drawBitmap(
    snake_head_pos.x * SNAKE_TILE_SIZE, snake_head_pos.y * SNAKE_TILE_SIZE,
    SNAKE_HEAD[snake_dir],
    SNAKE_TILE_SIZE, SNAKE_TILE_SIZE,
    0, BUF_REPLACE
  );

  if (snake_growth == 0) {
    // Очищаем область, где находился хвост
    display::oled.clear(
      snake_tail_pos.x * SNAKE_TILE_SIZE,
      snake_tail_pos.y * SNAKE_TILE_SIZE,
      snake_tail_pos.x * SNAKE_TILE_SIZE + SNAKE_TILE_SIZE - 1,
      snake_tail_pos.y * SNAKE_TILE_SIZE + SNAKE_TILE_SIZE - 1
    );
  
    int8_t tail_dir = field[snake_tail_pos.x][snake_tail_pos.y];
    auto tail_dir_vec = getDirVector((uint8_t)tail_dir);

    // Очищаем эту область на поле
    field[snake_tail_pos.x][snake_tail_pos.y] = -1;

    // Смещаем хвост
    snake_tail_pos.x = (snake_tail_pos.x + tail_dir_vec.x + SNAKE_FIELD_WIDTH) % SNAKE_FIELD_WIDTH;
    snake_tail_pos.y = (snake_tail_pos.y + tail_dir_vec.y + SNAKE_FIELD_HEIGHT) % SNAKE_FIELD_HEIGHT;

    // Рисуем хвост в новой точке
    display::oled.drawBitmap(
      snake_tail_pos.x * SNAKE_TILE_SIZE, snake_tail_pos.y * SNAKE_TILE_SIZE,
      SNAKE_TAIL[field[snake_tail_pos.x][snake_tail_pos.y]],
      SNAKE_TILE_SIZE, SNAKE_TILE_SIZE,
      0, BUF_REPLACE
    );
  } else {
    --snake_growth;
  }

  // Обновляем дисплей
  display::oled.update();

  return true;
}

const cvec2& Snake::getDirVector(uint8_t dir) {
  static const cvec2 ZERO = { 0, 0 };
  static const cvec2 UP_DIR = { 0, -1 };
  static const cvec2 RIGHT_DIR = { 1, 0 };
  static const cvec2 DOWN_DIR = { 0, 1 };
  static const cvec2 LEFT_DIR = { -1, 0 };
  switch (dir) {
    case SNAKE_DIR_UP:
      return UP_DIR;
    case SNAKE_DIR_RIGHT:
      return RIGHT_DIR;
    case SNAKE_DIR_DOWN:
      return DOWN_DIR;
    case SNAKE_DIR_LEFT:
      return LEFT_DIR;
    default:
      return ZERO;
  }
}

void Snake::startGame() {
  // Очищаем экран
  display::oled.clear();

  // Сбрасываем состояние игры
  _is_game_started = true;
  _last_update = millis();

  auto dir = getDirVector(INITIAL_SNAKE_DIR);

  // Сбрасываем состояние змейки
  snake_head_pos = { INITIAL_SNAKE_X, INITIAL_SNAKE_Y };
  snake_tail_pos = { 
    INITIAL_SNAKE_X - dir.x * (INITIAL_SNAKE_LENGTH - 1),
    INITIAL_SNAKE_Y - dir.y * (INITIAL_SNAKE_LENGTH - 1)
  };
  snake_dir = INITIAL_SNAKE_DIR;
  snake_growth = 0;

  // Очищаем игровое поле
  for (uint8_t x = 0; x < SNAKE_FIELD_WIDTH; ++x) {
    for (uint8_t y = 0; y < SNAKE_FIELD_HEIGHT; ++y) {
      field[x][y] = -1;
    }
  }

  // Заполняем телом змейки всю область
  cvec2 p = { INITIAL_SNAKE_X, INITIAL_SNAKE_Y };
  for (
    uint8_t i = 0;
    i < INITIAL_SNAKE_LENGTH;
    ++i, p.x -= dir.x, p.y -= dir.y
  ) {
    field[p.x][p.y] = snake_dir;
  }
}
