// Copyright (c) Tyler Veness

#include "globals.hpp"

#include <random>

#include <SFML/Graphics/Font.hpp>

std::mt19937& global_engine() {
  static std::mt19937 engine{std::random_device{}()};
  return engine;
}

sf::Font& global_font() {
  static sf::Font font{"data/arial.ttf"};
  return font;
}
