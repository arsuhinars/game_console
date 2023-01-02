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

// Спрайты еды
const uint8_t SNAKE_FOOD_0[] PROGMEM = {
	0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00, 
};
const uint8_t SNAKE_FOOD_1[] PROGMEM = {
	0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x00, 
};
const uint8_t SNAKE_FOOD_2[] PROGMEM = {
	0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 
};
const uint8_t* SNAKE_FOOD[] = {
  SNAKE_FOOD_0, SNAKE_FOOD_1, SNAKE_FOOD_2
};

const char* const SNAKE_MENU_ITEMS[] PROGMEM = {
  texts::START, texts::QUIT
};

Snake::Snake() : _menu(SNAKE_MENU_ITEMS, 2, FPSTR(texts::SNAKE_NAME)) {

}

bool Snake::update() {
  // Если игра не запущена, то рисуем меню
  if (!_is_game_started) {
    _menu.update();

    if (_menu.clickedItem() != -1) {
      if (_menu.clickedItem() == 0) {
        _menu.handleClickedItem();
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

  // Ищем подходящий спрайт для тела
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

  // Если змейка съела сама себя
  if (field[snake_head_pos.x][snake_head_pos.y] != -1) {
    _is_game_started = false;
    controls::resetStates();
    _menu.setTitleScale(1);
    _menu.setTitle(FPSTR(texts::YOU_LOSE));
    return true;    
  }

  // Обновляем текущий тайл головы
  field[snake_head_pos.x][snake_head_pos.y] = snake_dir;

  // Если змейка съела еду
  if (snake_head_pos.x == food_pos.x && snake_head_pos.y == food_pos.y) {
    // Увеличиваем змейку
    ++snake_growth;

    spawnFood();
    spawnExtraFood();
  } else {
    // Рисуем еду
    display::oled.drawBitmap(
      food_pos.x * SNAKE_TILE_SIZE, food_pos.y * SNAKE_TILE_SIZE,
      SNAKE_FOOD_0,
      SNAKE_TILE_SIZE, SNAKE_TILE_SIZE,
      0, BUF_REPLACE
    );
  }

  // Если змейка съела дополнительную еду
  if (extra_food_state > 0) {
    if (
      snake_head_pos.x == extra_food_pos.x &&
      snake_head_pos.y == extra_food_pos.y
    ) {
      // Увеличиваем змейку
      snake_growth += extra_food_state;
      extra_food_state = 0;

      spawnExtraFood();
    } else if (extra_food_state == 1) {
      extra_food_state = 0;
      display::oled.clear(
        extra_food_pos.x * SNAKE_TILE_SIZE,
        extra_food_pos.y * SNAKE_TILE_SIZE,
        extra_food_pos.x * SNAKE_TILE_SIZE + SNAKE_TILE_SIZE - 1,
        extra_food_pos.y * SNAKE_TILE_SIZE + SNAKE_TILE_SIZE - 1
      );
    } else {
      // Рисуем саму еду
      display::oled.drawBitmap(
        extra_food_pos.x * SNAKE_TILE_SIZE, extra_food_pos.y * SNAKE_TILE_SIZE,
        SNAKE_FOOD[(extra_food_state - 1) * 3 / SNAKE_EXTRA_FOOD_STEPS],
        SNAKE_TILE_SIZE, SNAKE_TILE_SIZE,
        0, BUF_REPLACE
      );

      --extra_food_state;
    }
  }

  // Рисуем спрайт головы
  display::oled.drawBitmap(
    snake_head_pos.x * SNAKE_TILE_SIZE, snake_head_pos.y * SNAKE_TILE_SIZE,
    SNAKE_HEAD[snake_dir],
    SNAKE_TILE_SIZE, SNAKE_TILE_SIZE,
    0, BUF_REPLACE
  );

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
  last_snake_dir = INITIAL_SNAKE_DIR;
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
    field[p.x][p.y] = INITIAL_SNAKE_DIR;

    // Вместе с этим и отрисовываем змейку
    uint8_t* sprite;
    if (i == 0) {
      sprite = SNAKE_HEAD[INITIAL_SNAKE_DIR];
    } else if (i == INITIAL_SNAKE_LENGTH - 1) {
      sprite = SNAKE_TAIL[INITIAL_SNAKE_DIR];
    } else {
      sprite = (INITIAL_SNAKE_DIR % 2 == 0) ? SNAKE_VERTICAL : SNAKE_HORIZONTAL;
    }

    display::oled.drawBitmap(
      p.x * SNAKE_TILE_SIZE, p.y * SNAKE_TILE_SIZE,
      sprite,
      SNAKE_TILE_SIZE, SNAKE_TILE_SIZE,
      0, BUF_REPLACE
    );
  }

  // Создаем еду
  spawnFood();
}

void Snake::spawnFood() {
  cvec2 last_suit_pos = { 0, 0 };
  for (uint8_t x = 0; x < SNAKE_FIELD_WIDTH; ++x) {
    for (uint8_t y = 0; y < SNAKE_FIELD_HEIGHT; ++y) {
      if (field[x][y] != -1) {
        continue;
      }

      last_suit_pos = { x, y };

      if (!random(SNAKE_FIELD_WIDTH * SNAKE_FIELD_HEIGHT)) {
        food_pos = last_suit_pos;
        return;
      }
    }
  }

  food_pos = last_suit_pos;
}

void Snake::spawnExtraFood() {
  if (extra_food_state > 0) {
    return;
  }

  if (random(SNAKE_EXTRA_FOOD_RATE)) {
    extra_food_state = 0;
    return;
  }

  cvec2 last_suit_pos = { 0, 0 };
  for (uint8_t x = 0; x < SNAKE_FIELD_WIDTH; ++x) {
    for (uint8_t y = 0; y < SNAKE_FIELD_HEIGHT; ++y) {
      if (field[x][y] != -1) {
        continue;
      }

      last_suit_pos = { x, y };

      if (!random(SNAKE_FIELD_WIDTH * SNAKE_FIELD_HEIGHT)) {
        extra_food_pos = last_suit_pos;
        extra_food_state = SNAKE_EXTRA_FOOD_STEPS;
        return;
      }
    }
  }

  extra_food_pos = last_suit_pos;
  extra_food_state = SNAKE_EXTRA_FOOD_STEPS;
}
