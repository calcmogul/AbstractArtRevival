// Copyright (c) Tyler Veness

#include "menus.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include "constants.hpp"
#include "globals.hpp"

void load_high_scores(std::vector<unsigned int>& high_score_list) {
  std::ifstream load_file{"scores.txt"};
  if (!load_file.is_open()) {
    return;
  }

  int i = 0;
  do {
    std::string str;
    std::getline(load_file, str);
    if (!load_file.eof()) {
      high_score_list.push_back(std::stoi(str));
    }
    ++i;
  } while (!load_file.eof() && i < 5);
}

void save_high_scores(std::vector<unsigned int>& high_score_list,
                      unsigned int score) {
  // Insert score into sorted vector (highest to lowest)
  high_score_list.insert(
      std::lower_bound(high_score_list.begin(), high_score_list.end(), score,
                       std::greater<>()),
      score);

  // Save only top 5 scores (highest to lowest)
  high_score_list.erase(high_score_list.begin() + 5, high_score_list.end());

  std::ofstream save_file{"scores.txt"};

  for (const auto& score : high_score_list) {
    save_file << std::format("{}\n", score);
  }
}

void display_high_scores(sf::RenderWindow& main_window,
                         const std::vector<unsigned int>& high_score_list,
                         const sf::Vector2f& center) {
  std::vector<sf::Text> score_text;

  // Populate high score table heading
  score_text.emplace_back(global_font(), "High Scores", 50);
  score_text.back().setStyle(sf::Text::Style::Bold);
  score_text.back().setOrigin(score_text.back().getLocalBounds().getCenter());
  score_text.back().setPosition(center + sf::Vector2f{0.f, -50.f});

  // Populate high scores
  for (size_t i = 0; i < high_score_list.size(); ++i) {
    score_text.emplace_back(global_font(), std::to_string(high_score_list[i]),
                            30);
    score_text.back().setOrigin(score_text.back().getLocalBounds().getCenter());
    score_text.back().setPosition(center + sf::Vector2f{0.f, 10.f + 40.f * i});
  }

  while (main_window.isOpen()) {
    while (auto event = main_window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        main_window.close();
      } else if (auto mouse_event =
                     event->getIf<sf::Event::MouseButtonReleased>();
                 mouse_event &&
                 mouse_event->button == sf::Mouse::Button::Left) {
        return;
      }
    }

    main_window.clear(BACKGROUND_COLOR);
    for (auto& text : score_text) {
      main_window.draw(text);
    }

    main_window.display();
  }
}

void display_main_menu(sf::RenderWindow& main_window,
                       const sf::Vector2f& center) {
  std::string choice_made;

  sf::Text title{global_font(), "Abstract Art Revival", 50};
  title.setStyle(sf::Text::Style::Bold);
  title.setOrigin(title.getLocalBounds().getCenter());
  title.setPosition(center + sf::Vector2f{0.f, -100.f});

  // Populate menu items
  std::vector<sf::Text> menu_items;
  menu_items.emplace_back(global_font(), "New Game", 30);
  menu_items.emplace_back(global_font(), "High Scores", 30);
  menu_items.emplace_back(global_font(), "Exit", 30);

  // Set menu item positions
  for (size_t i = 0; i < menu_items.size(); ++i) {
    menu_items[i].setOrigin(
        {menu_items[i].getLocalBounds().getCenter().x, 0.f});

    if (i == 0) {
      menu_items[i].setPosition(center);
    } else {
      menu_items[i].setPosition(
          {center.x, menu_items[i - 1].getPosition().y +
                         menu_items[i - 1].getLocalBounds().size.y + 20.f});
    }
  }

  while (main_window.isOpen()) {
    while (auto event = main_window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        main_window.close();
      } else if (auto mouse_event =
                     event->getIf<sf::Event::MouseButtonReleased>();
                 mouse_event &&
                 mouse_event->button == sf::Mouse::Button::Left) {
        if (choice_made == "New Game") {
          return;
        } else if (choice_made == "High Scores") {
          std::vector<unsigned int> high_score_list;
          load_high_scores(high_score_list);
          display_high_scores(main_window, high_score_list, center);
        } else if (choice_made == "Exit") {
          main_window.close();
        }
      }
    }

    sf::Vector2f mouse =
        main_window.mapPixelToCoords(sf::Mouse::getPosition(main_window));
    for (size_t i = 0; i < menu_items.size(); ++i) {
      auto& menu_item = menu_items[i];

      if (menu_item.getGlobalBounds().contains(mouse)) {
        menu_item.setFillColor(sf::Color{128, 128, 128});
        menu_item.setOutlineColor(sf::Color::White);
        menu_item.setOutlineThickness(4.f);
        choice_made = menu_item.getString();
      } else {
        menu_item.setFillColor(sf::Color::White);
        menu_item.setOutlineColor(sf::Color::Transparent);
        menu_item.setOutlineThickness(0.f);
      }
    }

    main_window.clear(BACKGROUND_COLOR);

    main_window.draw(title);
    for (auto& item : menu_items) {
      main_window.draw(item);
    }

    main_window.display();
  }
}

bool display_pause_menu(sf::RenderWindow& main_window,
                        const sf::Vector2f& center) {
  std::string choice_made;

  // Populate menu items
  std::vector<sf::Text> menu_items;
  menu_items.emplace_back(global_font(), "Resume", 30);
  menu_items.emplace_back(global_font(), "Reset Game", 30);
  menu_items.emplace_back(global_font(), "High Scores", 30);
  menu_items.emplace_back(global_font(), "Exit", 30);

  // Set menu item positions
  for (size_t i = 0; i < menu_items.size(); ++i) {
    menu_items[i].setOrigin(
        {menu_items[i].getLocalBounds().getCenter().x, 0.f});

    if (i == 0) {
      menu_items[i].setPosition(center);
    } else {
      menu_items[i].setPosition(
          {center.x, menu_items[i - 1].getPosition().y +
                         menu_items[i - 1].getLocalBounds().size.y + 20.f});
    }
  }

  while (main_window.isOpen()) {
    while (auto event = main_window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        main_window.close();
      } else if (auto mouse_event =
                     event->getIf<sf::Event::MouseButtonReleased>();
                 mouse_event &&
                 mouse_event->button == sf::Mouse::Button::Left) {
        if (choice_made == "Resume") {
          return false;
        } else if (choice_made == "Reset Game") {
          return true;
        } else if (choice_made == "High Scores") {
          std::vector<unsigned int> high_score_list;
          load_high_scores(high_score_list);
          display_high_scores(main_window, high_score_list, center);
        } else if (choice_made == "Exit") {
          main_window.close();
        }
      }
    }

    sf::Vector2f mouse =
        main_window.mapPixelToCoords(sf::Mouse::getPosition(main_window));
    for (size_t i = 0; i < menu_items.size(); ++i) {
      auto& menu_item = menu_items[i];

      if (menu_item.getGlobalBounds().contains(mouse)) {
        menu_item.setFillColor(sf::Color{128, 128, 128});
        menu_item.setOutlineColor(sf::Color::White);
        menu_item.setOutlineThickness(4.f);
        choice_made = menu_item.getString();
      } else {
        menu_item.setFillColor(sf::Color::White);
        menu_item.setOutlineColor(sf::Color::Transparent);
        menu_item.setOutlineThickness(0.f);
      }
    }

    main_window.clear(BACKGROUND_COLOR);
    for (auto& item : menu_items) {
      main_window.draw(item);
    }

    main_window.display();
  }

  return false;
}

void game_over(sf::RenderWindow& main_window, unsigned int score,
               const sf::Vector2f& center) {
  sf::Text game_over_text_1{global_font(), "GAME OVER", 30};
  game_over_text_1.setOrigin(game_over_text_1.getLocalBounds().getCenter());
  game_over_text_1.setPosition(center + sf::Vector2f{0.f, -50.f});

  sf::Text game_over_text_2{global_font(), "Click to continue...", 30};
  game_over_text_2.setOrigin(game_over_text_2.getLocalBounds().getCenter());
  game_over_text_2.setPosition(center + sf::Vector2f{0.f, 50.f});

  while (main_window.isOpen()) {
    while (auto event = main_window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        main_window.close();
      } else if (auto mouse_event =
                     event->getIf<sf::Event::MouseButtonReleased>();
                 mouse_event &&
                 mouse_event->button == sf::Mouse::Button::Left) {
        std::vector<unsigned int> high_score_list;
        load_high_scores(high_score_list);
        save_high_scores(high_score_list, score);
        display_high_scores(main_window, high_score_list, center);
        return;
      }
    }

    main_window.clear(BACKGROUND_COLOR);
    main_window.draw(game_over_text_1);
    main_window.draw(game_over_text_2);

    main_window.display();
  }
}
