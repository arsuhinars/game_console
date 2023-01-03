#include "controls.hpp"
#include "display.hpp"
#include "utils.hpp"
#include "menu.hpp"

using namespace utils;

#define MENU_ICON_SIZE    8
#define MENU_ITEM_HEIGHT  (DISPLAY_FONT_HEIGHT + MENU_ITEMS_SPACING)

// Иконки указателей в меню размером 8x8
const uint8_t chevron_right[] PROGMEM = {
  0x00, 0x00, 0x81, 0xc3, 0x66, 0x3c, 0x18, 0x00
};
const uint8_t chevron_left[] PROGMEM = {
  0x00, 0x18, 0x3c, 0x66, 0xc3, 0x81, 0x00, 0x00
};

Menu::Menu(
  const char* const *items,
  uint8_t items_count,
  const __FlashStringHelper* title,
  uint8_t title_scale
) {
  _items = items;
  _items_count = items_count;
  _title = title;
  _title_scale = title_scale;
}

Menu::~Menu() { }

void Menu::setTitle(const __FlashStringHelper* title) {
  _title = title;
  _need_redraw = true;
}

void Menu::setTitleScale(uint8_t title_scale) {
  _title_scale = title_scale;
  _need_redraw = true;
}

void Menu::update() {
  // Ждем пока не будет обработан нажатый элемент
  if (_is_item_clicked) {
    return;
  }

  // Вычисляем высоту заголовка
  int title_height = DISPLAY_FONT_HEIGHT * _title_scale + MENU_ITEMS_SPACING;
  // Вычисляем максимальное кол-во элементов на экране
  auto max_display_items = static_cast<uint8_t>(
    _title != nullptr ? 
    (DISPLAY_HEIGHT - title_height) / MENU_ITEM_HEIGHT :
    DISPLAY_HEIGHT / MENU_ITEM_HEIGHT
  );

  // Обрабатываем нажатие вверх
  if (controls::up_button.click() && _selected_item > 0) {
    if (_selected_item == _items_scroll) {
      --_items_scroll;
    }

    --_selected_item;
    _need_redraw = true;
  }

  // Обрабатываем нажатие вниз
  if (controls::down_button.click() && _selected_item < _items_count - 1) {
    if (_selected_item == (_items_scroll + max_display_items - 1)) {
      ++_items_scroll;
    }

    ++_selected_item;
    _need_redraw = true;
  }

  // Обрабатываем выбор элемента
  if (controls::button_a.click()) {
    _is_item_clicked = true;
  }

  // Делаем отрисовку только при необходимости
  if (!_need_redraw) {
    return;
  }
  _need_redraw = false;

  display::oled.textMode(BUF_REPLACE);

  int item_y = 0;

  if (_title != nullptr) {
    size_t title_length = rus_strlen_P(reinterpret_cast<PGM_P>(_title));
    int title_x = DISPLAY_WIDTH / 2 - title_length * DISPLAY_FONT_WIDTH / 2 * _title_scale;

    // Отрисовываем заголовок
    display::oled.setScale(_title_scale);
    display::oled.clear(0, 0, title_x - 1, DISPLAY_FONT_HEIGHT * _title_scale - 1);
    display::oled.clear(
      title_x + title_length * DISPLAY_FONT_WIDTH * _title_scale,
      0,
      DISPLAY_WIDTH - 1,
      DISPLAY_FONT_HEIGHT * _title_scale - 1
    );
    display::oled.clear(
      0,
      DISPLAY_FONT_HEIGHT * _title_scale,
      DISPLAY_WIDTH - 1,
      title_height - 1
    );
    display::oled.setCursorXY(title_x, 0);
    display::oled.print(_title);

    item_y = title_height;
  }

  // Отрисовываем пункты меню
  display::oled.setScale(1);
  for (size_t i = _items_scroll; i < (int)_items_count; ++i) {
    size_t item_length = rus_strlen_P((PGM_P)pgm_read_word(&_items[i]));
    int item_x = DISPLAY_WIDTH / 2 - item_length * DISPLAY_FONT_WIDTH / 2;

    display::oled.clear(0, item_y, item_x - 1, item_y + MENU_ITEM_HEIGHT - 1);
    display::oled.clear(
      item_x + item_length * DISPLAY_FONT_WIDTH,
      item_y,
      DISPLAY_WIDTH - 1,
      item_y + MENU_ITEM_HEIGHT - 1
    );
    display::oled.setCursorXY(item_x, item_y);
    display::oled.print(FPSTR(pgm_read_word(&_items[i])));

    if (i == _selected_item) {
      // Отрисовываем курсор
      display::oled.drawBitmap(
        item_x - MENU_ICON_SIZE - MENU_CURSOR_SPACING,
        item_y,
        chevron_right,
        MENU_ICON_SIZE, MENU_ICON_SIZE
      );
      display::oled.drawBitmap(
        item_x + item_length * DISPLAY_FONT_WIDTH + MENU_CURSOR_SPACING,
        item_y,
        chevron_left,
        MENU_ICON_SIZE, MENU_ICON_SIZE
      );
    }

    item_y += MENU_ITEM_HEIGHT;
    if (item_y >= DISPLAY_HEIGHT) {
      break;
    }
  }

  // Очищаем оставшуюся часть
  display::oled.clear(0, item_y, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

  if (_items_count > max_display_items) {
    uint8_t n = _items_count - max_display_items + 1;

    // Вычисляем размер ползунка
    int slider_size = max(
      DISPLAY_HEIGHT / n,
      MENU_SLIDER_MIN_SIZE
    );
    // и его положение на экране
    int slider_y = (DISPLAY_HEIGHT - slider_size) * _items_scroll / (n - 1);

    // Отрисовываем ползунок
    display::oled.rect(
      DISPLAY_WIDTH - 1 - MENU_SLIDER_THICKNESS,
      slider_y,
      DISPLAY_WIDTH - 1,
      slider_y + slider_size - 1
    );
  }

  // Обновляем дисплей
  display::oled.update();
}

int Menu::clickedItem() {
  return _is_item_clicked ? _selected_item : -1;
}

void Menu::handleClickedItem() {
  _is_item_clicked = false;
}

void Menu::forceRedraw() {
  _need_redraw = true;
}