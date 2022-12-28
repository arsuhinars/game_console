#include "controls.hpp"
#include "display.hpp"
#include "games.hpp"
#include "menu.hpp"

// Меню с выбором игр
Menu main_menu = Menu(
  reinterpret_cast<const __FlashStringHelper* const*>(games_names),
  games_count
);

// Текущая запущенная игра
Game* current_game = nullptr;

void setup() {
  Serial.begin(9600);

  // Инициализируем экран 
  display::init();
}

void loop() {
  // Обновляем ввод
  controls::update();

  if (current_game == nullptr) {
    // Отрисовываем текущее меню, если не запущена игра
    main_menu.update();

    // Обрабатываем запуск игры
    if (main_menu.clickedItem() != -1) {
      current_game = games[main_menu.clickedItem()]();

      main_menu.handleClickedItem();
    }
  } else {
    current_game->update();
  }
}
