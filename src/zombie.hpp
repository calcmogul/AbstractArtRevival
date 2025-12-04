// Copyright (c) Tyler Veness

#pragma once

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include "constants.hpp"
#include "globals.hpp"

/// Zombie type.
enum class ZombieType { Small, Big };

/// Zombie entity.
class Zombie {
 public:
  /// Constructs a zombie.
  ///
  /// @param position Initial position.
  /// @param type Zombie type.
  Zombie(const sf::Vector2f& position, ZombieType type) {
    this->position = position;

    switch (type) {
      case ZombieType::Small:
        velocity = {25.f, 0.f};

        health = 200.f;
        max_health = 200.f;
        xp = 100;
        break;
      case ZombieType::Big:
        velocity = {50.f, 0.f};

        health = 500.f;
        max_health = 500.f;
        xp = 300;
        break;
    }

    body_shape.setPosition(position);
    body_shape.setFillColor(sf::Color::Transparent);
    body_shape.setOutlineColor(BODY_COLOR);
    body_shape.setRadius(std::max(0.1f, (max_health - health) / 10.f));
    body_shape.setOrigin(body_shape.getGeometricCenter());
    body_shape.setOutlineThickness(health / 10.f);
  }

  Zombie(Zombie&&) = default;
  Zombie& operator=(Zombie&&) = default;

  /// Sets the position.
  ///
  /// @param position The position.
  void set_position(const sf::Vector2f& position) {
    this->position = position;
    body_shape.setPosition(position);
  }

  /// Returns the position.
  const sf::Vector2f& get_position() const { return position; }

  /// Returns the velocity.
  const sf::Vector2f& get_velocity() const { return velocity; }

  /// Returns the zombie's health.
  float get_health() const { return health; }

  /// Decrements the zombie's health by the given amount.
  ///
  /// @param decrement The amount to decrement.
  void decrement_health(float decrement) { health -= decrement; }

  /// Returns the amount of experience killing this zombie would award.
  uint32_t get_xp() const { return xp; }

  /// Returns the zombie's radius for collision detection.
  float get_radius() const { return max_health / 10.f; }

  /// Returns the global bounds for collision detection.
  sf::FloatRect get_global_bounds() const {
    return sf::FloatRect{position - sf::Vector2f{get_radius(), get_radius()},
                         sf::Vector2f{2.f * get_radius(), 2.f * get_radius()}};
  }

  /// Steps simulation forward by one frame.
  ///
  /// @param frame_duration Frame duration in seconds.
  /// @param player_position Player position.
  /// @param player_velocity Player velocity.
  void update_movement(float frame_duration,
                       const sf::Vector2f& player_position,
                       const sf::Vector2f& player_velocity) {
    const sf::FloatRect ZOMBIE_BOUNDS{
        MAP_BOUNDS.position + sf::Vector2f{get_radius(), get_radius()},
        MAP_BOUNDS.size - sf::Vector2f{get_radius(), get_radius()}};

    float player_speed = player_velocity.length();
    float zombie_speed = velocity.length();

    // Aim zombie at player
    velocity = {zombie_speed, (player_position - position).angle()};

    float player_cos = player_velocity.x / player_speed;
    float player_sin = player_velocity.y / player_speed;
    float zombie_cos = velocity.x / zombie_speed;
    float zombie_sin = velocity.y / zombie_speed;

    // Aim zombie at player while leading target
    float a = player_cos * zombie_cos - player_sin * zombie_sin;
    if (std::abs(player_speed / zombie_speed * a) < 1.f) {
      velocity = velocity.rotatedBy(
          sf::radians(std::asin(player_speed / zombie_speed * a)));
    }

    sf::Vector2f delta_position = velocity * frame_duration;

    if (ZOMBIE_BOUNDS.contains(position + delta_position)) {
      position += delta_position;
      body_shape.move(delta_position);
    }
  }

  /// Draws zombie on main window.
  ///
  /// @param main_window Main window.
  void draw(sf::RenderWindow& main_window) {
    body_shape.setRadius(std::max(0.1f, (max_health - health) / 10.f));
    body_shape.setOrigin(body_shape.getGeometricCenter());
    body_shape.setOutlineThickness(health / 10.f);

    main_window.draw(body_shape);
  }

  /// Spawns zombies at the edge of the map.
  ///
  /// @param zombies The list of active zombies.
  /// @param xp The player's accrued experience (proportional to spawn rate).
  static void spawn(std::vector<Zombie>& zombies, uint32_t xp) {
    uint32_t max_zombies = std::min(xp / 100 + 10, 1000u);

    // Stop spawning zombies if at max
    if (zombies.size() == max_zombies) {
      return;
    }

    // Don't spawn zombie until timer has elapsed
    if (spawn_clock.getElapsedTime().asSeconds() <
        SPAWN_PERIOD * zombies.size() / max_zombies) {
      return;
    }

    spawn_clock.restart();

    // 1 in 10 chance of spawning a big zombie
    if (std::uniform_int_distribution<>{0, 9}(global_engine()) == 0) {
      zombies.emplace_back(sf::Vector2f{}, ZombieType::Big);
    } else {
      zombies.emplace_back(sf::Vector2f{}, ZombieType::Small);
    }
    auto& new_zombie = zombies.back();
    float radius = new_zombie.get_radius();

    // Spawn zombie at edge of map
    sf::Vector2f position;
    int edge = std::uniform_int_distribution<>{0, 3}(global_engine());
    if (edge == 0) {
      // Spawn at right edge
      position.x = MAP_DIMS.x - radius;
      std::uniform_real_distribution<> y_distr{radius, MAP_DIMS.y - radius};
      position.y = y_distr(global_engine());
    } else if (edge == 1) {
      // Spawn at top edge
      std::uniform_real_distribution<> x_distr{radius, MAP_DIMS.x - radius};
      position.x = x_distr(global_engine());
      position.y = radius;
    } else if (edge == 2) {
      // Spawn at left edge
      position.x = radius;
      std::uniform_real_distribution<> y_distr{radius, MAP_DIMS.y - radius};
      position.y = y_distr(global_engine());
    } else {
      // Spawn at bottom edge
      std::uniform_real_distribution<> x_distr{radius, MAP_DIMS.x - radius};
      position.x = x_distr(global_engine());
      position.y = MAP_DIMS.y - radius;
    }

    new_zombie.set_position(position);
  }

  /// Resets spawn clock.
  static void reset() { spawn_clock.restart(); }

 private:
  static constexpr sf::Color BODY_COLOR{40, 60, 40};

  /// Spawn period in seconds
  static constexpr float SPAWN_PERIOD = 0.5f;

  sf::Vector2f position;
  sf::Vector2f velocity;

  /// Zombie's current health.
  float health;

  /// Zombie's maximum health.
  float max_health;

  /// XP this zombie is worth if killed.
  uint32_t xp;

  static inline sf::Clock spawn_clock;

  sf::CircleShape body_shape;
};
