// Copyright (c) Tyler Veness

#pragma once

#include <array>
#include <memory>
#include <numbers>
#include <string>
#include <utility>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

#include "bullet.hpp"
#include "globals.hpp"
#include "random_angle.hpp"
#include "weapon_type.hpp"

// NB: To add a new weapon type:
//
//   * Add enum value to WeaponType.
//   * Add initial ammo amount to get_initial_ammo().
//   * Add case to switch-case in Weapon constructor that sets weapon stats.
//   * Add case to switch-case in Weapon::make_bullet() that creates bullet SFML
//     shape and returns a new bullet.
//   * Add case to switch-case in Weapon::draw() that draws weapon symbol.
//   * (optional) Add features unique to this weapon type to main.cpp
//     * Add branch to bullet firing code if there's more than one bullet per
//       shot.
//     * Add branch to bullet-zombie collision if there's special handling of
//       collisions (e.g., chain/area damage).

/// Returns initial ammo for the given weapon.
constexpr int get_initial_ammo(WeaponType type) {
  constexpr std::array INITIAL_AMMO{1000, 250, 200, 10, 20, 500, 10};
  return INITIAL_AMMO[std::to_underlying(type)];
}

class Weapon {
 public:
  WeaponType type;
  int ammo;
  float fire_period;
  float accuracy;
  float bullet_speed;
  int bullet_damage;

  /// Constructs a weapon with the default amount of initial ammo.
  ///
  /// @param type Weapon type.
  explicit Weapon(WeaponType type) : type{type}, ammo{get_initial_ammo(type)} {
    using enum WeaponType;

    switch (type) {
      case HANDGUN:
        fire_period = 0.5f;
        accuracy = 1.f;
        bullet_speed = 1000.f;
        bullet_damage = 200;
        break;
      case MACHINE_GUN:
        fire_period = 1.f / 15.f;
        accuracy = 0.98f;
        bullet_speed = 2000.f;
        bullet_damage = 50;
        break;
      case FLAMETHROWER:
        fire_period = 0.02f;
        accuracy = 0.9f;
        bullet_speed = 200.f;
        bullet_damage = 200;
        break;
      case LASER:
        fire_period = 1.f;
        accuracy = 1.f;
        bullet_speed = 1000.f;
        bullet_damage = 2000;
        break;
      case SHOTGUN:
        fire_period = 1.f;
        accuracy = 0.95f;
        bullet_speed = 1500.f;
        bullet_damage = 75;
        break;
      case MINIGUN:
        fire_period = 0.01f;
        accuracy = 0.9f;
        bullet_speed = 2500.f;
        bullet_damage = 100;
        break;
      case ROCKET_LAUNCHER:
        fire_period = 2.f;
        accuracy = 1.f;
        bullet_speed = 1000.f;
        bullet_damage = 2000;
        break;
    }
  }

  /// Constructs a weapon.
  ///
  /// @param type Weapon type.
  /// @param ammo Initial ammo.
  Weapon(WeaponType type, int ammo) : Weapon(type) { this->ammo = ammo; }

  /// Fires a bullet from the gun.
  ///
  /// @param position Initial bullet position.
  /// @param rotation Bullet rotation as a 2D unit vector.
  /// @return The bullet instance.
  Bullet make_bullet(const sf::Vector2f& position,
                     const sf::Vector2f& rotation) {
    using enum WeaponType;

    sf::Vector2f velocity =
        bullet_speed * rotation.rotatedBy(random_angle(accuracy));

    switch (type) {
      case HANDGUN: {
        auto body_shape =
            std::make_unique<sf::RectangleShape>(sf::Vector2f{10.f, 1.f});
        body_shape->setFillColor(sf::Color::White);

        return Bullet{position,
                      velocity,
                      type,
                      bullet_damage,
                      std::move(body_shape),
                      BulletShape::RECTANGLE};
      }
      case MACHINE_GUN: {
        auto body_shape =
            std::make_unique<sf::RectangleShape>(sf::Vector2f{10.f, 1.f});
        body_shape->setFillColor(sf::Color::Yellow);

        return Bullet{position,
                      velocity,
                      type,
                      bullet_damage,
                      std::move(body_shape),
                      BulletShape::RECTANGLE};
      }
      case FLAMETHROWER: {
        auto body_shape = std::make_unique<sf::CircleShape>(5.f);
        body_shape->setFillColor(sf::Color::Yellow);
        body_shape->setOutlineThickness(3.f);
        body_shape->setOutlineColor(sf::Color::Red);

        return Bullet{position,
                      velocity,
                      type,
                      bullet_damage,
                      std::move(body_shape),
                      BulletShape::CIRCLE};
      }
      case LASER: {
        auto body_shape =
            std::make_unique<sf::RectangleShape>(sf::Vector2f{20.f, 2.f});
        body_shape->setFillColor(sf::Color::White);

        return Bullet{position,
                      velocity,
                      type,
                      bullet_damage,
                      std::move(body_shape),
                      BulletShape::RECTANGLE};
      }
      case SHOTGUN: {
        auto body_shape =
            std::make_unique<sf::RectangleShape>(sf::Vector2f{10.f, 1.f});
        body_shape->setFillColor(sf::Color::Magenta);

        return Bullet{position,
                      velocity,
                      type,
                      bullet_damage,
                      std::move(body_shape),
                      BulletShape::RECTANGLE};
      }
      case MINIGUN: {
        auto body_shape =
            std::make_unique<sf::RectangleShape>(sf::Vector2f{20.f, 3.f});
        body_shape->setFillColor(sf::Color::Red);

        return Bullet{position,
                      velocity,
                      type,
                      bullet_damage,
                      std::move(body_shape),
                      BulletShape::RECTANGLE};
      }
      case ROCKET_LAUNCHER: {
        auto body_shape = std::make_unique<sf::ConvexShape>(7);
        body_shape->setPoint(0, {0.f, 0.f});
        body_shape->setPoint(1, {-6.f, -4.f});
        body_shape->setPoint(2, {-13.5f, -4.f});
        body_shape->setPoint(3, {-18.f, -7.f});
        body_shape->setPoint(4, {-18.f, 7.f});
        body_shape->setPoint(5, {-13.5f, 4.f});
        body_shape->setPoint(6, {-6.f, 4.f});
        body_shape->setFillColor(sf::Color::Red);

        return Bullet{position,
                      velocity,
                      type,
                      bullet_damage,
                      std::move(body_shape),
                      BulletShape::CONVEX};
      }
      default:
        std::unreachable();
    }
  }

  /// Draws weapon symbol next to player on main window.
  ///
  /// @param main_window Main window.
  /// @param player_position Player position.
  void draw(sf::RenderWindow& main_window,
            const sf::Vector2f& player_position) {
    using enum WeaponType;

    sf::Vector2f symbol_center{player_position.x + 30.f, player_position.y};

    constexpr sf::Color BACKGROUND_COLOR{200, 200, 200};
    sf::RectangleShape background{{20.f, 20.f}};
    background.setOrigin(background.getGeometricCenter());
    background.setPosition(symbol_center);
    background.setFillColor(BACKGROUND_COLOR);
    main_window.draw(background);

    // Draw weapon symbol
    switch (type) {
      case HANDGUN: {
        sf::RectangleShape barrel{{15.f, 7.5f}};
        barrel.setOrigin(barrel.getGeometricCenter());
        barrel.setPosition(symbol_center + sf::Vector2f{1.5f, -3.f});
        barrel.setFillColor(sf::Color::Black);
        main_window.draw(barrel);

        sf::RectangleShape grip{{15.f, 5.f}};
        grip.setOrigin(grip.getGeometricCenter());
        grip.setPosition(symbol_center + sf::Vector2f{-4.5f, 0.f});
        grip.setRotation(sf::radians(-0.4f * std::numbers::pi_v<float>));
        grip.setFillColor(sf::Color::Black);
        main_window.draw(grip);
        break;
      }
      case MACHINE_GUN: {
        sf::RectangleShape magazine{{4.f, 2.f}};
        magazine.setOrigin(magazine.getGeometricCenter());
        magazine.setPosition(symbol_center + sf::Vector2f{1.f, 1.f});
        magazine.setRotation(sf::radians(0.3f * std::numbers::pi_v<float>));
        magazine.setFillColor(sf::Color::Black);
        main_window.draw(magazine);

        sf::RectangleShape barrel{{15.f, 3.f}};
        barrel.setOrigin(barrel.getGeometricCenter());
        barrel.setPosition(symbol_center + sf::Vector2f{1.5f, -1.5f});
        barrel.setFillColor(sf::Color{60, 60, 60});
        main_window.draw(barrel);

        sf::RectangleShape grip{{7.f, 4.5f}};
        grip.setOrigin(grip.getGeometricCenter());
        grip.setPosition(symbol_center + sf::Vector2f{-5.5f, 0.f});
        grip.setRotation(sf::radians(-0.025f * std::numbers::pi_v<float>));
        grip.setFillColor(sf::Color::Black);
        main_window.draw(grip);
        break;
      }
      case FLAMETHROWER: {
        constexpr sf::Color TAIL_BACK_ORANGE{170 * 9 / 10, 85 * 9 / 10, 0};
        constexpr sf::Color TAIL_FRONT_ORANGE{170 * 11 / 10, 85 * 11 / 10, 0};
        constexpr sf::Color HEAD_ORANGE{170 * 12 / 10, 85 * 12 / 10, 0};

        sf::CircleShape tail_back{3.f};
        tail_back.setOrigin(tail_back.getGeometricCenter());
        tail_back.setPosition(symbol_center + sf::Vector2f{-6.5f, 0.f});
        tail_back.setFillColor(TAIL_BACK_ORANGE);
        main_window.draw(tail_back);

        sf::CircleShape tail_front{4.f};
        tail_front.setOrigin(tail_front.getGeometricCenter());
        tail_front.setPosition(symbol_center + sf::Vector2f{-3.f, 0.f});
        tail_front.setFillColor(TAIL_FRONT_ORANGE);
        main_window.draw(tail_front);

        sf::CircleShape head{6.f};
        head.setOrigin(head.getGeometricCenter());
        head.setPosition(symbol_center + sf::Vector2f{3.f, 0.f});
        head.setFillColor(HEAD_ORANGE);
        main_window.draw(head);
        break;
      }
      case LASER: {
        sf::CircleShape caution{10.f, 3};
        caution.setOrigin(caution.getGeometricCenter());
        caution.setPosition(symbol_center + sf::Vector2f{0.f, 2.f});
        caution.setFillColor(sf::Color::Yellow);
        caution.setOutlineThickness(1.f);
        caution.setOutlineColor(sf::Color::Black);
        main_window.draw(caution);

        sf::CircleShape source{2.f};
        source.setOrigin(source.getGeometricCenter());
        source.setPosition(symbol_center + sf::Vector2f{0.f, 2.f});
        source.setFillColor(sf::Color::Black);
        main_window.draw(source);

        sf::RectangleShape spike{{8.f, 1.f}};
        spike.setOrigin(spike.getGeometricCenter());
        spike.setPosition(symbol_center + sf::Vector2f{0.f, 2.f});
        spike.setFillColor(sf::Color::Black);
        for (int i = 0; i < 6; ++i) {
          spike.setRotation(sf::radians(i * std::numbers::pi_v<float> / 6.f));
          main_window.draw(spike);
        }

        spike.setOrigin({0.f, spike.getGeometricCenter().y});
        spike.setRotation(sf::radians(0.f));
        spike.setSize({7.f, 1.f});
        main_window.draw(spike);
        break;
      }
      case SHOTGUN: {
        sf::RectangleShape barrel{{15.f, 3.f}};
        barrel.setOrigin(barrel.getGeometricCenter());
        barrel.setPosition(symbol_center + sf::Vector2f{1.5f, -1.5f});
        barrel.setFillColor(sf::Color{60, 60, 60});
        main_window.draw(barrel);

        sf::RectangleShape grip{{7.f, 4.5f}};
        grip.setOrigin(grip.getGeometricCenter());
        grip.setPosition(symbol_center + sf::Vector2f{-5.5f, 0.f});
        grip.setRotation(sf::radians(-0.025f * std::numbers::pi_v<float>));
        grip.setFillColor(sf::Color{60, 30, 0});
        main_window.draw(grip);
        break;
      }
      case MINIGUN: {
        constexpr int BARRELS = 5;

        sf::CircleShape center_brace{1.5f};
        center_brace.setOrigin(center_brace.getGeometricCenter());
        center_brace.setPosition(symbol_center);
        center_brace.setFillColor(sf::Color::Black);
        main_window.draw(center_brace);

        sf::CircleShape outer_brace{6.f};
        outer_brace.setOrigin(outer_brace.getGeometricCenter());
        outer_brace.setPosition(symbol_center);
        outer_brace.setFillColor(sf::Color::Transparent);
        outer_brace.setOutlineColor(sf::Color::Black);
        outer_brace.setOutlineThickness(2.f);
        main_window.draw(outer_brace);

        sf::CircleShape barrel{1.f};
        barrel.setOrigin(barrel.getGeometricCenter());
        barrel.setFillColor(BACKGROUND_COLOR);
        barrel.setOutlineColor(sf::Color::Black);
        barrel.setOutlineThickness(2.f);
        for (size_t i = 0; i < BARRELS; ++i) {
          barrel.setPosition(
              symbol_center +
              sf::Vector2f{6.f, sf::radians(2.f * std::numbers::pi_v<float> /
                                            BARRELS * i)});
          main_window.draw(barrel);
        }

        break;
      }
      case ROCKET_LAUNCHER: {
        sf::ConvexShape shape{7};
        shape.setPosition(symbol_center + sf::Vector2f{0.f, -9.f});
        shape.setRotation(sf::radians(-std::numbers::pi_v<float> / 2.f));
        shape.setPoint(0, {0.f, 0.f});
        shape.setPoint(1, {-6.f, -4.f});
        shape.setPoint(2, {-13.5f, -4.f});
        shape.setPoint(3, {-18.f, -7.f});
        shape.setPoint(4, {-18.f, 7.f});
        shape.setPoint(5, {-13.5f, 4.f});
        shape.setPoint(6, {-6.f, 4.f});
        shape.setFillColor(sf::Color::Red);
        main_window.draw(shape);
        break;
      }
    }

    // Draw ammo count
    sf::Text ammo_count{global_font(), std::to_string(ammo), 10};
    ammo_count.setOrigin({ammo_count.getLocalBounds().getCenter().x, 0.f});
    ammo_count.setPosition(
        {player_position.x + 30.f, player_position.y + 10.f});

    main_window.draw(ammo_count);
  }
};
