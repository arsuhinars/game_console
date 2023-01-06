#include <Arduino.h>

#include "display.hpp"
#include "controls.hpp"
#include "texts.hpp"
#include "dino.hpp"

using namespace utils;

// Размеры всех спрайтов
#define DINO_SIZE_X       24
#define DINO_SIZE_Y       24

#define DINO_DUCK_SIZE_X  32
#define DINO_DUCK_SIZE_Y  16

#define DINO_DEAD_SIZE_X  24
#define DINO_DEAD_SIZE_Y  24

#define CACTUS_0_SIZE_X   11
#define CACTUS_0_SIZE_Y   24
#define CACTUS_0_REAL_Y   20

#define CACTUS_1_SIZE_X   17
#define CACTUS_1_SIZE_Y   32
#define CACTUS_1_REAL_Y   25

#define BIRD_SIZE_X       23
#define BIRD_SIZE_Y       24
#define BIRD_REAL_Y       18

#define DINO_GROUND_Y     (DISPLAY_HEIGHT - DINO_GROUND_HEIGHT - 1)

// Спрайты динозаврика
const uint8_t DINO_0[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xF6, 0xFE, 0xFE, 0xFE, 0x7E, 0x7E, 0x7E, 0x7E, 0x7C, 0x00, 0x00, 
	0x00, 0x00, 0xFE, 0xF8, 0xF0, 0xE0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x09, 0x19, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0xFF, 0xBF, 0x1F, 0x0F, 0x0F, 0x1F, 0x17, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};
const uint8_t DINO_1[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xF6, 0xFE, 0xFE, 0xFE, 0x7E, 0x7E, 0x7E, 0x7E, 0x7C, 0x00, 0x00, 
	0x00, 0x00, 0xFE, 0xF8, 0xF0, 0xE0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x09, 0x19, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x2F, 0x0F, 0x1F, 0xFF, 0x87, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};
const uint8_t DINO_DUCK_0[] PROGMEM = {
	0x00, 0x38, 0x70, 0xF0, 0xF0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xF0, 0xF8, 0xD8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x37, 0x2F, 0x0F, 0x0F, 0x0F, 0xFF, 0xBF, 0x1F, 0x0F, 0x0F, 0x3F, 0x2F, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x05, 0x05, 0x05, 0x01, 0x01, 0x00, 
};
const uint8_t DINO_DUCK_1[] PROGMEM = {
	0x00, 0x38, 0x70, 0xF0, 0xF0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xF0, 0xF8, 0xD8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xF7, 0xBF, 0x1F, 0x0F, 0x0F, 0x3F, 0x2F, 0x2F, 0x0F, 0x0F, 0x3F, 0x2F, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x05, 0x05, 0x05, 0x01, 0x01, 0x00, 
};
const uint8_t DINO_DEAD[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xE2, 0xEA, 0xE2, 0xFE, 0xFE, 0x7E, 0x7E, 0x7E, 0x7E, 0x7C, 0x00, 0x00, 
	0x00, 0x00, 0xFE, 0xF8, 0xF0, 0xE0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x09, 0x19, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0xFF, 0xBF, 0x1F, 0x0F, 0xFF, 0x87, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

// Спрайты двух видов кактусов
const uint8_t CACTUS_0[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xE0, 0x00, 0x80, 0xC0, 0x80, 
	0xFE, 0xFF, 0xFE, 0x00, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0x7F, 
	0x03, 0x07, 0x0F, 0x0E, 0xFF, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0x00, 
};
const uint8_t CACTUS_1[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xF0, 0xF8, 0xF8, 0xF0, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0x80, 
	0x07, 0x0F, 0x1F, 0x3F, 0x3C, 0x3C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xE0, 0xFF, 0x7F, 0x3F, 0x1F, 
	0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

// Спрайты птеродактеля
const uint8_t BIRD_0[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x40, 0x60, 0x70, 0x78, 0x7C, 0x7E, 0x7E, 0x78, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x1F, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x02, 0x02, 0x00, 
};
const uint8_t BIRD_1[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x40, 0x60, 0x70, 0x78, 0x7C, 0x7E, 0x7E, 0x78, 0xE1, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x02, 0x02, 0x00, 
};

Dino::Dino() {
  resetGame();

  // Сбрасываем состояния кнопок
  controls::resetStates();
}

bool Dino::update() {
  // Ждем пока не наступит время обновления игры
  if (millis() - _last_time <= DINO_UPDATE_STEP) {
    return true;
  }
  _last_time = millis();

  // Если игра не запущена
  if (!_is_game_started) {
    // Рисуем меню
    if (!_is_menu_rendered) {
      _is_menu_rendered = true;

      // Отрисовываем игру
      renderGame();

      // Отрисовываем заголовок
      display::oled.textMode(BUF_ADD);
      display::oled.setScale(2);
      display::oled.setCursorXY(
        DISPLAY_WIDTH / 2 - utils::rus_strlen_P(texts::DINO_NAME) * DISPLAY_FONT_WIDTH,
        DINO_TITLE_MARGIN
      );
      display::oled.print(FPSTR(texts::DINO_NAME));

      // Рисуем подсказку о начале игры
      display::oled.setScale(1);
      display::oled.setCursorXY(
        DISPLAY_WIDTH / 2 - utils::rus_strlen_P(texts::START_HINT) * DISPLAY_FONT_WIDTH / 2,
        DINO_TITLE_MARGIN + DISPLAY_FONT_HEIGHT * 2
      );
      display::oled.print(FPSTR(texts::START_HINT));

      // Рисуем подсказку о выходе из игры
      display::oled.setCursorXY(
        DISPLAY_WIDTH / 2 - utils::rus_strlen_P(texts::EXIT_HINT) * DISPLAY_FONT_WIDTH / 2,
        DINO_TITLE_MARGIN + DISPLAY_FONT_HEIGHT * 3
      );
      display::oled.print(FPSTR(texts::EXIT_HINT));

      // Выводим все на экран
      display::oled.update();
    }

    // Обрабатываем нажатия
    if (controls::up_button.press() || controls::button_a.press()) {
      resetGame();
      _is_game_started = true;
    } else if (controls::button_b.press()) {
      return false;
    }
  } else {  // Если игра запущена
    _old_dino_y = _dino_y;
    
    // Обновляем гравитацию
    if (controls::down_button.state()) {
      _dino_vel += DINO_DUCK_GRAVITY * DINO_UPDATE_STEP / 1000;
    } else {
      _dino_vel += DINO_GRAVITY * DINO_UPDATE_STEP / 1000;
    }

    // Тип прыжка, 0 - нет, 1 - обычный, 2 - высокий
    uint8_t jump_type = 0;
    if (controls::up_button.release() || controls::button_a.release()) {
      jump_type = 1;
    }
    else if (controls::up_button.hold() || controls::button_a.hold()) {
      jump_type = 2;
    }

    // Если динозаврик на земле и не в присяди
    if (jump_type > 0 && _dino_y >= DINO_GROUND_Y && !controls::down_button.state()) {
      _dino_vel = jump_type == 1 ? -DINO_JUMP_SPEED : -DINO_HIGH_JUMP_SPEED;
    }

    // Перемещаяем в соответствии со скоростью
    _dino_y += _dino_vel * DINO_UPDATE_STEP / 1000;

    // Обрабатываем столкновение с землей
    if (_dino_y > DINO_GROUND_Y) {
      _dino_y = DINO_GROUND_Y;
      _dino_vel = 0;
    }

    // Если нужно спавнить препятствие
    if (_game_counter >= _next_obstacle_val) {
      _next_obstacle_val = _game_counter + random(DINO_MIN_OBSTACLE_RATE, DINO_MAX_OBSTACLE_RATE + 1);

      if (!random(DINO_BIRD_SPAWN_RATE)) {
        // Спавним птицу
        spawnBird();
      } else {
        spawnCacti();
      }
    }

    // Увеличиваем счетчик кадров
    if (++_frame_counter > DINO_BASE_SCORE_RATE) {
      // Увеличиваем счет игры в соотвествии со скоростью
      ++_game_counter;
      // Сбрасываем счетчик кадров
      _frame_counter = 0;
    }

    // Отрисовываем игру
    renderGame();
  }

  return true;
}

void Dino::renderGame() {
  // Очищаем область для динозаврика
  display::oled.clear(
    DINO_POS_X,
    _old_dino_y - max(DINO_SIZE_Y, DINO_DUCK_SIZE_Y) + 1,
    DINO_POS_X + max(DINO_SIZE_X, DINO_DUCK_SIZE_X) - 1,
    _old_dino_y
  );
  
  // Выводим счет на экран
  display::oled.textMode(BUF_REPLACE);
  display::oled.setScale(1);
  display::oled.setCursorXY(DINO_SCORE_MARGIN, 0);
  uint8_t score_length = (uint8_t)display::oled.print(_game_counter);

  // Обновляем и отрисовываем кактусы
  for (uint8_t i = 0; i < DINO_MAX_CACTI; ++i) {
    if (!_cacti[i].is_active) {
      continue;
    }

    // Обновляем положение кактуса
    _cacti[i].old_pos_x = _cacti[i].pos_x;
    _cacti[i].pos_x -= DINO_BASE_SPEED * DINO_UPDATE_STEP / 1000;

    ucvec2 size = (_cacti[i].type == 0) ?
      ucvec2 { CACTUS_0_SIZE_X, CACTUS_0_SIZE_Y } :
      ucvec2 { CACTUS_1_SIZE_X, CACTUS_1_SIZE_Y };

    // Если кактус за границами экрана
    if (_cacti[i].pos_x + size.x < 0) {
      _cacti[i].is_active = false;
      continue;
    }

    // Рисуем кактус
    display::oled.clear(
      _cacti[i].old_pos_x,
      DINO_GROUND_Y - size.y + 1,
      _cacti[i].old_pos_x + size.x,
      DINO_GROUND_Y
    );
    display::oled.drawBitmap(
      _cacti[i].pos_x, DINO_GROUND_Y - size.y + 1,
      (_cacti[i].type == 0 ? CACTUS_0 : CACTUS_1),
      size.x, size.y,
      0, BUF_ADD
    );
  }

  // Обновляем и отрисовываем птиц
  for (uint8_t i = 0; i < DINO_MAX_BIRDS; ++i) {
    if (!_birds[i].is_active) {
      continue;
    }

    // Обновляем положение птицы
    _birds[i].old_pos_x = _birds[i].pos.x;
    _birds[i].pos.x -= (DINO_BIRD_MOVE_SPEED + DINO_BASE_SPEED) * DINO_UPDATE_STEP / 1000;

    // Если птица за границами экрана
    if (_birds[i].pos.x + BIRD_SIZE_X + 1 < 0) {
      _birds[i].is_active = false;
      continue;
    }

    // Рисуем птицу
    display::oled.clear(
      _birds[i].old_pos_x,
      _birds[i].pos.y - BIRD_SIZE_Y + 1,
      _birds[i].old_pos_x + BIRD_SIZE_X,
      _birds[i].pos.y
    );
    display::oled.drawBitmap(
      _birds[i].pos.x, _birds[i].pos.y - BIRD_SIZE_Y + 1,
      (_game_counter % (DINO_BIRD_ANIM_STEP * 2) < DINO_BIRD_ANIM_STEP ? BIRD_0 : BIRD_1),
      BIRD_SIZE_X, BIRD_SIZE_Y,
      0, BUF_ADD
    );
  }

  // Рисуем динозаврика
  if (!controls::down_button.state()) {
    // Если динозаврик стоит
    display::oled.drawBitmap(
      DINO_POS_X,
      _dino_y - DINO_SIZE_Y + 1,
      (_game_counter % (DINO_ANIM_STEP * 2) < DINO_ANIM_STEP) ? DINO_0 : DINO_1,
      DINO_SIZE_X, DINO_SIZE_Y,
      0, BUF_ADD
    );
  } else {
    // Если динозаврик сидит
    display::oled.drawBitmap(
      DINO_POS_X,
      _dino_y - DINO_DUCK_SIZE_Y + 1,
      (_game_counter % (DINO_ANIM_STEP * 2) < DINO_ANIM_STEP) ? DINO_DUCK_0 : DINO_DUCK_1,
      DINO_DUCK_SIZE_X, DINO_DUCK_SIZE_Y,
      0, BUF_ADD
    );
  }
  
  // Обновляем все области на экране
  display::oled.update(
    DINO_SCORE_MARGIN,
    0,
    DINO_SCORE_MARGIN + score_length * DISPLAY_FONT_WIDTH,
    DISPLAY_FONT_HEIGHT
  );
  display::oled.update(
    DINO_POS_X,
    min(_old_dino_y, _dino_y) - max(DINO_SIZE_Y, DINO_DUCK_SIZE_Y) + 2,
    DINO_POS_X + max(DINO_SIZE_X, DINO_DUCK_SIZE_X),
    max(_old_dino_y, _dino_y) + 1
  );
  for (uint8_t i = 0; i < DINO_MAX_CACTI; ++i) {
    if (_cacti[i].is_active) {
      ucvec2 size = (_cacti[i].type == 0) ?
        ucvec2 { CACTUS_0_SIZE_X, CACTUS_0_SIZE_Y } :
        ucvec2 { CACTUS_1_SIZE_X, CACTUS_1_SIZE_Y };
      display::oled.update(
        _cacti[i].pos_x,
        DINO_GROUND_Y - size.y + 2,
        _cacti[i].old_pos_x + size.x,
        DINO_GROUND_Y
      );
    }
  }
  for (uint8_t i = 0; i < DINO_MAX_BIRDS; ++i) {
    if (_birds[i].is_active) {
      display::oled.update(
        _birds[i].pos.x,
        _birds[i].pos.y - BIRD_SIZE_Y + 1,
        _birds[i].old_pos_x + BIRD_SIZE_X + 20,
        _birds[i].pos.y + 1
      );
    }
  }
}

void Dino::resetGame() {
  // Сбрасываем все параметры игры
  _is_menu_rendered = false;
  _is_game_started = false;
  _last_time = millis();
  _next_obstacle_val = 0;
  _old_dino_y = DINO_GROUND_Y;
  _dino_y = DINO_GROUND_Y;
  _dino_vel = 0;
  _frame_counter = 0;
  _game_counter = 0;

  // Сбрасываем все объекты
  for (uint8_t i = 0; i < DINO_MAX_CACTI; ++i) {
    _cacti[i].is_active = false;
  }
  for (uint8_t i = 0; i < DINO_MAX_BIRDS; ++i) {
    _birds[i].is_active = false;
  }

  // Очищаем экран
  display::oled.clear();
  // Рисуем землю и выводим её на экран
  display::oled.fastLineH(DINO_GROUND_Y + 1, 0, DISPLAY_WIDTH - 1);
  // Отрисовываем кадр игры
  renderGame();
  // Обновляем экран
  display::oled.update();
}

void Dino::spawnCacti() {
  int8_t cactus_index = -1;
  // Ищем свободный объект кактуса
  for (int8_t j = 0; j < DINO_MAX_CACTI; ++j) {
    if (!_cacti[j].is_active) {
      cactus_index = j;
      break;
    }
  }

  // Если больше нет свободных объектов
  if (cactus_index == -1) {
    return;
  }

  // Спавним кактус
  _cacti[cactus_index] = {
    true,
    0,
    DISPLAY_WIDTH + 1,
    random(2)
  };
}

void Dino::spawnBird() {
  // Ищем свободный объект птицы
  int8_t bird_index = -1;
  for (int8_t i = 0; i < DINO_MAX_BIRDS; ++i) {
    if (!_birds[i].is_active) {
      bird_index = i;
      break;
    }
  }

  // Если нет свободных объектов
  if (bird_index == -1) {
    return;
  }

  int8_t bird_y = random(
    DISPLAY_HEIGHT - DINO_BIRD_SPAWN_HEIGHT + BIRD_SIZE_Y - 1,
    DINO_GROUND_Y
  );
  // Создаем птицу
  _birds[bird_index] = {
    true,
    0,
    { DISPLAY_WIDTH + 1, bird_y }
  };
}
