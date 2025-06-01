// Copyright (c) Tyler Veness

#pragma once

#include <random>
#include <vector>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include "constants.hpp"
#include "globals.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "weapon_type.hpp"

/**
 * Weapon crate entity.
 */
class WeaponCrate {
 public:
  /**
   * Constructs a weapon crate.
   *
   * @param position Initial position.
   */
  explicit WeaponCrate(const sf::Vector2f& position, WeaponType type)
      : position{position}, type{type}, ammo{get_initial_ammo(type)} {
    body_shape.setOrigin(body_shape.getGeometricCenter());
    body_shape.setPosition(position);
    body_shape.setFillColor(sf::Color{60, 30, 0});
    body_shape.setOutlineThickness(OUTER_WIDTH);
    body_shape.setOutlineColor(sf::Color{100, 50, 0});

    spawn_clock.restart();
  }

  /**
   * Returns the position.
   */
  const sf::Vector2f& get_position() const { return position; }

  /**
   * Returns the weapon type this crate contains.
   */
  WeaponType get_type() const { return type; }

  /**
   * Returns the amount of ammunition this crate contains.
   */
  int get_ammo() const { return ammo; }

  /**
   * Returns true if weapon crate lifetime clock expired.
   */
  bool expired() const {
    return lifetime_clock.getElapsedTime().asSeconds() > 30.f;
  }

  /**
   * Returns the size of this crate for collision detection.
   */
  sf::Vector2f get_size() const { return sf::Vector2f{WIDTH, WIDTH}; }

  /**
   * Returns the global bounds for collision detection.
   */
  sf::FloatRect get_global_bounds() const {
    return sf::FloatRect{position - get_size() / 2.f, get_size()};
  }

  /**
   * Draws weapon crate on main window.
   *
   * @param main_window Main window.
   */
  void draw(sf::RenderWindow& main_window) { main_window.draw(body_shape); }

  /**
   * Spawns weapon crates at regular intervals near the player.
   *
   * @param weapon_crates The list of active weapon crates.
   * @param player The player entity.
   */
  static void spawn(std::vector<WeaponCrate>& weapon_crates,
                    const Player& player) {
    if (spawn_clock.getElapsedTime().asSeconds() > SPAWN_PERIOD) {
      std::uniform_real_distribution<float> width_distr{-SCREEN_DIMS.x / 2.f,
                                                        SCREEN_DIMS.x / 2.f};
      std::uniform_real_distribution<float> height_distr{-SCREEN_DIMS.y / 2.f,
                                                         SCREEN_DIMS.y / 2.f};
      std::uniform_int_distribution<> weapon_distr{1, NUM_WEAPONS - 1};

      sf::Vector2f position;
      do {
        position = {
            std::clamp(player.get_position().x + width_distr(global_engine()),
                       WIDTH / 2.f, MAP_DIMS.x - WIDTH / 2.f),
            std::clamp(player.get_position().y + height_distr(global_engine()),
                       WIDTH / 2.f, MAP_DIMS.y - WIDTH / 2.f)};
      } while (player.get_global_bounds().contains(position));
      weapon_crates.emplace_back(
          position, static_cast<WeaponType>(weapon_distr(global_engine())));

      spawn_clock.restart();
    }
  }

  /**
   * Resets spawn clock.
   */
  static void reset() { spawn_clock.restart(); }

 private:
  static constexpr float INNER_WIDTH = 10.f;
  static constexpr float OUTER_WIDTH = 4.f;
  static constexpr float WIDTH = INNER_WIDTH + OUTER_WIDTH;

  static constexpr sf::Color INNER_COLOR{60, 30, 0};
  static constexpr sf::Color OUTER_COLOR{100, 50, 0};

  /// Spawn period in seconds.
  static constexpr float SPAWN_PERIOD = 10.f;

  sf::Vector2f position;

  WeaponType type;
  int ammo;

  sf::Clock lifetime_clock;

  static inline sf::Clock spawn_clock;

  sf::RectangleShape body_shape{{10.f, 10.f}};
};
