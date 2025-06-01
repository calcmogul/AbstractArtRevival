// Copyright (c) Tyler Veness

#pragma once

#include <random>

#include <SFML/Graphics/Font.hpp>

/**
 * Returns the application-wide random number engine.
 */
std::mt19937& global_engine();

/**
 * Returns the application-wide font.
 */
sf::Font& global_font();
