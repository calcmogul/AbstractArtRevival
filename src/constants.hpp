// Copyright (c) Tyler Veness

#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

/// Screen dimensions in pixels.
constexpr sf::Vector2f SCREEN_DIMS{1280.f, 720.f};

/// Map dimensions in pixels.
constexpr sf::Vector2f MAP_DIMS{4000.f, 3000.f};

/// Map rectangle in pixels.
constexpr sf::FloatRect MAP_BOUNDS{{0.f, 0.f}, MAP_DIMS};

/// Window background color
constexpr sf::Color BACKGROUND_COLOR = sf::Color::Black;

/// Ground color
constexpr sf::Color GROUND_COLOR{80, 80, 80};
