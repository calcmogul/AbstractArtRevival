// Copyright (c) Tyler Veness

#pragma once

#include <stdint.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <utility>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include "constants.hpp"
#include "weapon.hpp"
#include "weapon_type.hpp"

/// Player entity.
class Player {
 public:
  /// Constructs a Player.
  ///
  /// @param position Initial position.
  explicit Player(const sf::Vector2f& position) {
    this->position = position;

    stamina_arc.setPosition(position);
    stamina_arc.setFillColor(sf::Color::Blue);

    body_shape.setPosition(position);
    body_shape.setRadius(max_health / 10.f);
    body_shape.setOrigin(body_shape.getGeometricCenter());

    body_shader_state.shader = &body_shader;

    center_shape.setPosition(position);
    center_shape.setFillColor(sf::Color::Black);
    center_shape.setRadius((max_health - health) / 10.f);
    center_shape.setOrigin(center_shape.getGeometricCenter());
  }

  Player(Player&&) = default;
  Player& operator=(Player&&) = default;

  /// Returns the position.
  const sf::Vector2f& get_position() const { return position; }

  /// Sets the velocity.
  ///
  /// @param speed The player's speed.
  /// @param angle The player's velocity direction.
  void set_velocity(float speed, sf::Angle angle) {
    velocity = {speed * std::cos(angle.asRadians()),
                speed * std::sin(angle.asRadians())};
  }

  /// Returns the velocity.
  const sf::Vector2f& get_velocity() const { return velocity; }

  /// Returns the player's health.
  float get_health() const { return health; }

  /// Decrements the player's health by the given amount.
  ///
  /// @param decrement The amount to decrement.
  void decrement_health(float decrement) { health -= decrement; }

  /// Returns the player's accrued experience.
  uint32_t get_xp() const { return xp; }

  /// Increments the player's accrued experience by the given amount.
  ///
  /// @param increment The amount to increment.
  void increment_xp(uint32_t increment) { xp += increment; }

  /// Returns the player's radius for collision detection.
  float get_radius() const { return max_health / 10.f; }

  /// Returns the global bounds for collision detection.
  sf::FloatRect get_global_bounds() const {
    return sf::FloatRect{position - sf::Vector2f{get_radius(), get_radius()},
                         sf::Vector2f{2.f * get_radius(), 2.f * get_radius()}};
  }

  /// Steps simulation forward by one frame.
  ///
  /// @param frame_duration Frame duration in seconds.
  /// @param direction The direction the player will move as a 2D unit vector.
  /// @param sprint Whether the player will attempt to sprint.
  void update_movement(float frame_duration, const sf::Vector2f& direction,
                       bool sprint) {
    const sf::FloatRect PLAYER_BOUNDS{
        MAP_BOUNDS.position + sf::Vector2f{get_radius(), get_radius()},
        MAP_BOUNDS.size - sf::Vector2f{get_radius(), get_radius()}};

    if (direction.x != 0.f || direction.y != 0.f) {
      velocity = {speed * direction.x, speed * direction.y};
    } else {
      velocity = {0.f, 0.f};
    }

    if (sprint && can_sprint && stamina > 0.f &&
        (velocity.x != 0.f || velocity.y != 0.f)) {
      velocity *= 4.f;
      stamina -= 20.f * frame_duration;
    }

    sf::Vector2f delta_position = velocity * frame_duration;

    if (PLAYER_BOUNDS.contains(position + delta_position)) {
      position += delta_position;
      stamina_arc.move(delta_position);
      body_shape.move(delta_position);
      center_shape.move(delta_position);
    }

    if (stamina <= 0.f) {
      can_sprint = false;
      stamina = 0.f;
    } else if (stamina > 25.f) {
      can_sprint = true;
    }
    stamina = std::min(stamina + 10.f * frame_duration, 100.f);
  }

  /// Draws player on main window.
  ///
  /// @param main_window Main window.
  void draw(sf::RenderWindow& main_window) {
    for (size_t i = 0; i < 30; ++i) {
      auto angle = sf::radians(i / 29.f * 2.0 * std::numbers::pi_v<float> *
                               stamina / max_stamina);
      angle -= sf::radians(std::numbers::pi_v<float> / 2.f);
      stamina_arc.setPoint(i, {get_radius() + 5.f, angle});
    }
    stamina_arc.setPoint(30, {0.f, 0.f});

    if (can_sprint) {
      stamina_arc.setFillColor(sf::Color::Blue);
    } else {
      stamina_arc.setFillColor(CANT_SPRINT_COLOR);
    }

    body_shape.setRadius(max_health / 10.f);
    body_shape.setOrigin(body_shape.getGeometricCenter());

    center_shape.setRadius((max_health - health) / 10.f);
    center_shape.setOrigin(center_shape.getGeometricCenter());

    get_current_weapon().draw(main_window, position);

    // Update shader inputs
    body_shader.setUniform("texture", sf::Shader::CurrentTexture);
    body_shader.setUniform("center",
                           position - main_window.getView().getCenter() +
                               sf::Vector2f{main_window.getSize()} / 2.f);

    main_window.draw(stamina_arc);
    main_window.draw(body_shape, body_shader_state);
    main_window.draw(center_shape);
  }

  Weapon& get_current_weapon() { return weapons[current_weapon]; }

  /// Returns the weapon with the given type.
  ///
  /// @param type The weapon type.
  Weapon& get_weapon(WeaponType type) {
    return weapons[std::to_underlying(type)];
  }

  /// Switch to previous weapon.
  void switch_to_previous_weapon() {
    if (current_weapon == 0) {
      current_weapon = NUM_WEAPONS - 1;
    } else {
      --current_weapon;
    }
  }

  /// Switch to next weapon.
  void switch_to_next_weapon() {
    current_weapon = (current_weapon + 1) % NUM_WEAPONS;
  }

  /// Switch to the given weapon.
  ///
  /// @param type The weapon to switch to.
  void switch_weapon(WeaponType type) {
    using enum WeaponType;

    current_weapon = std::to_underlying(type);
  }

  /// Returns true and resets timer if player can fire another bullet.
  bool try_fire() {
    if (fire_period_clock.getElapsedTime().asSeconds() >
        get_current_weapon().fire_period) {
      fire_period_clock.restart();
      return true;
    } else {
      return false;
    }
  }

 private:
  static constexpr sf::Color CANT_SPRINT_COLOR{128, 128, 255};

  sf::Vector2f position;
  sf::Vector2f velocity;

  float speed = 50.f;

  sf::Clock fire_period_clock;

  /// Player's current health.
  float health = 100.f;

  /// Player's maximum health.
  float max_health = 100.f;

  /// Player's stamina.
  float stamina = 100.f;

  /// Player's maximum stamina.
  float max_stamina = 100.f;

  /// Whether the player can sprint.
  bool can_sprint = true;

  /// Player's experience.
  uint32_t xp = 0;

  std::array<Weapon, 7> weapons{
      Weapon{WeaponType::HANDGUN},           Weapon{WeaponType::MACHINE_GUN, 0},
      Weapon{WeaponType::FLAMETHROWER, 0},   Weapon{WeaponType::LASER, 0},
      Weapon{WeaponType::SHOTGUN, 0},        Weapon{WeaponType::MINIGUN, 0},
      Weapon{WeaponType::ROCKET_LAUNCHER, 0}};
  int current_weapon = 0;

  sf::ConvexShape stamina_arc{31};

  sf::CircleShape body_shape;
  sf::Shader body_shader{std::string_view{R"(
#version 330

uniform sampler2D texture;
uniform vec2 center;

// Based on https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative
float f(float h, float s, float v, float n) {
  float k = mod(n + h / 60.f, 6.f);
  return v - s * v * clamp(min(k, 4.f - k), 0.f, 1.f);
}

vec4 hsv_to_rgb(float h, float s, float v, float a) {
  return vec4(f(h, s, v, 5), f(h, s, v, 3), f(h, s, v, 1), a);
}

void main() {
  float angle = atan(gl_FragCoord.y - center.y, gl_FragCoord.x - center.x);  // NOLINT
  float alpha = texture2D(texture, gl_FragCoord.xy).a;

  gl_FragColor = hsv_to_rgb(degrees(angle) + 180.f, 1.f, 1.f, alpha);
})"},
                         sf::Shader::Type::Fragment};
  sf::RenderStates body_shader_state;

  sf::CircleShape center_shape;
};
