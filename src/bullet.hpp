// Copyright (c) Tyler Veness

#pragma once

#include <memory>
#include <utility>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include "constants.hpp"
#include "weapon_type.hpp"

enum class BulletShape { CIRCLE, RECTANGLE, CONVEX };

/// Bullet's maximum lifetime in seconds.
constexpr float BULLET_MAX_LIFETIME = 1.f;

/// Bullet entity.
class Bullet {
 public:
  /// Constructs a Bullet of the given weapon type.
  ///
  /// @param position Initial position.
  /// @param velocity Initial velocity.
  /// @param type Weapon type this bullet came from.
  /// @param damage Damage.
  /// @param body_shape SFML body shape.
  /// @param bullet_shape Bullet shape.
  Bullet(const sf::Vector2f& position, const sf::Vector2f& velocity,
         WeaponType type, int damage, std::unique_ptr<sf::Shape> body_shape,
         BulletShape bullet_shape)
      : position{position},
        velocity{velocity},
        type{type},
        damage{damage},
        body_shape{std::move(body_shape)},
        bullet_shape{bullet_shape} {
    this->body_shape->setOrigin(this->body_shape->getGeometricCenter());
    this->body_shape->setPosition(position);
    this->body_shape->setRotation(velocity.angle());
  }

  Bullet(Bullet&&) = default;
  Bullet& operator=(Bullet&&) = default;

  /// Returns the position.
  const sf::Vector2f& get_position() const { return position; }

  /// Returns the velocity.
  const sf::Vector2f& get_velocity() const { return velocity; }

  /// Returns the rotation.
  sf::Angle get_rotation() const { return body_shape->getRotation(); }

  /// Returns the weapon type this bullet came from.
  const WeaponType& get_type() const { return type; }

  /// Returns the damage this bullet is capable of.
  float get_damage() const { return damage; }

  /// Returns the global bounds for collision detection.
  sf::FloatRect get_global_bounds() const {
    return body_shape->getGlobalBounds();
  }

  /// Returns the bullet shape.
  BulletShape get_shape() const { return bullet_shape; }

  /// Returns true if bullet lifetime clock expired.
  bool expired() const {
    return lifetime_clock.getElapsedTime().asSeconds() > BULLET_MAX_LIFETIME;
  }

  /// Steps simulation forward by one frame.
  ///
  /// @param frame_duration Frame duration in seconds.
  void update_movement(float frame_duration) {
    sf::Vector2f delta_position = velocity * frame_duration;

    if (MAP_BOUNDS.contains(position + delta_position)) {
      position += delta_position;
      body_shape->move(delta_position);
    }

    if (type == WeaponType::LASER) {
      float bullet_time = lifetime_clock.getElapsedTime().asSeconds();
      auto shape = static_cast<sf::RectangleShape*>(body_shape.get());
      shape->setSize({bullet_time * 1000.f, 2.f});
    } else if (type == WeaponType::FLAMETHROWER) {
      float bullet_time = lifetime_clock.getElapsedTime().asSeconds();
      auto shape = static_cast<sf::CircleShape*>(body_shape.get());

      // Fade flamethrower bullet to black by the time it despawns
      float decay_factor = 1.f - bullet_time / BULLET_MAX_LIFETIME;
      shape->setFillColor(sf::Color{static_cast<uint8_t>(255.f * decay_factor),
                                    static_cast<uint8_t>(255.f * decay_factor),
                                    0});
      shape->setOutlineColor(
          sf::Color{static_cast<uint8_t>(255.f * decay_factor), 0, 0});
    }
  }

  /// Draws bullet on main window.
  ///
  /// @param main_window Main window.
  void draw(sf::RenderWindow& main_window) { main_window.draw(*body_shape); }

 private:
  sf::Vector2f position;
  sf::Vector2f velocity;

  WeaponType type;
  int damage;

  sf::Clock lifetime_clock;

  std::unique_ptr<sf::Shape> body_shape;
  BulletShape bullet_shape;
};
