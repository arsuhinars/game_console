#pragma once

#include <Arduino.h>

// Размеры отступов между пунктами меню
#define MENU_ITEMS_SPACING 4
// Отступ между текстом пункта меню и курсором
#define MENU_CURSOR_SPACING 4

// Класс для создания и отображения наэкранного меню.
class Menu {
public:
  /**
  * Конструктор класса
  * @param items массив F() строк элементов меню
  * @param items_count размер массива элементов
  * @param title F() строка с текстом заголовка меню. Не является обязательной.
  */
  explicit Menu(
    const __FlashStringHelper** items,
    uint8_t items_count,
    const __FlashStringHelper* title = nullptr
  );
  ~Menu();

  /// Метод для отрисовки и обновления меню на экране
  void update();

  /**
  * Возвращает индекс элемента при клике на него.
  * Если элемент не был нажат, то возвращает -1.
  * Данная функция используется в основном цикле после вызов update()
  */
  int clickedItem();

  /**
  * Метод обработки и сброса состояния нажатого элемента. 
  * Необходимо вызывать после его обработки.
  */
  void handleClickedItem();

private:
  const __FlashStringHelper** _items;
  uint8_t _items_count;
  const __FlashStringHelper* _title;

  bool _need_redraw = true;
  uint8_t _items_scroll = 0;
  uint8_t _selected_item = 0;
  bool _is_item_clicked = false;
};
