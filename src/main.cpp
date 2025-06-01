// Copyright (c) Tyler Veness

#include <stdint.h>

#include <cmath>
#include <deque>
#include <memory>
#include <utility>
#include <vector>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "bullet.hpp"
#include "collision_detector.hpp"
#include "constants.hpp"
#include "menus.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "weapon_crate.hpp"
#include "weapon_type.hpp"
#include "zombie.hpp"

int main() {
  sf::RenderWindow main_window{sf::VideoMode{sf::Vector2u{SCREEN_DIMS}},
                               "Abstract Art Revival", sf::Style::Default,
                               sf::State::Fullscreen};
  main_window.setFramerateLimit(60);

  sf::View view;
  view.setViewport(sf::FloatRect{{0.f, 0.f}, {1.f, 1.f}});
  view.setCenter(SCREEN_DIMS / 2.f);
  main_window.setView(view);

  display_main_menu(main_window, SCREEN_DIMS / 2.f);

  sf::Clock frame_clock;

  std::deque<Bullet> bullets;
  std::vector<WeaponCrate> weapon_crates;
  Player player{SCREEN_DIMS / 2.f};
  std::vector<Zombie> zombies;

  // Make ground tile
  sf::RenderTexture ground_render_texture{{20, 20}};
  ground_render_texture.setRepeated(true);
  {
    ground_render_texture.clear(GROUND_COLOR);

    sf::RectangleShape rect{{2.f, 2.f}};
    rect.setFillColor(sf::Color{60, 60, 60});

    rect.setPosition({2.f, 3.f});
    ground_render_texture.draw(rect);

    rect.setPosition({8.f, 13.f});
    ground_render_texture.draw(rect);

    rect.setPosition({15.f, 6.f});
    ground_render_texture.draw(rect);

    rect.setPosition({18.f, 16.f});
    ground_render_texture.draw(rect);

    ground_render_texture.display();
  }

  // Make ground sprite as repeating ground tile
  sf::Sprite ground_sprite{ground_render_texture.getTexture(),
                           {{0, 0}, sf::Vector2i{MAP_BOUNDS.size}}};

  while (main_window.isOpen()) {
    float frame_duration = frame_clock.restart().asSeconds();

    while (auto event = main_window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        main_window.close();
      } else if (auto key_event = event->getIf<sf::Event::KeyPressed>()) {
        if (key_event->code == sf::Keyboard::Key::Q) {
          player.switch_to_previous_weapon();
        } else if (key_event->code == sf::Keyboard::Key::E) {
          player.switch_to_next_weapon();
        }
      }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
        player.try_fire()) {
      auto angle =
          main_window.mapPixelToCoords(sf::Mouse::getPosition(main_window)) -
          player.get_position();
      if (angle.x != 0.f || angle.y != 0.f) {
        angle /= angle.length();
      }

      if (player.get_current_weapon().ammo > 0) {
        if (player.get_current_weapon().type == WeaponType::SHOTGUN) {
          for (int i = 0; i < 15; ++i) {
            bullets.emplace_back(player.get_current_weapon().make_bullet(
                player.get_position(), angle));
          }
        } else {
          bullets.emplace_back(player.get_current_weapon().make_bullet(
              player.get_position(), angle));
        }

        --player.get_current_weapon().ammo;
      }
    }

    // Update movement for all moving entities
    for (auto& bullet : bullets) {
      bullet.update_movement(frame_duration);
    }

    sf::Vector2f player_direction;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
      player_direction.y -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
      player_direction.y += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
      player_direction.x -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
      player_direction.x += 1.f;
    }
    if (player_direction.x != 0.f || player_direction.y != 0.f) {
      player_direction /= player_direction.length();
    }
    player.update_movement(
        frame_duration, player_direction,
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space));

    for (auto& zombie : zombies) {
      zombie.update_movement(frame_duration, player.get_position(),
                             player.get_velocity());
    }

    view.setCenter(player.get_position());
    main_window.setView(view);

    WeaponCrate::spawn(weapon_crates, player);
    Zombie::spawn(zombies, player.get_xp());

    // Check for bullet -> zombie collisions
    for (size_t i = 0; i < bullets.size(); ++i) {
      // Index is used here instead of iterator since insertion can invalidate
      // all iterators
      auto& bullet = bullets[i];

      for (auto it = zombies.begin(); it != zombies.end();) {
        auto& zombie = *it;

        // If bounding boxes don't intersect, skip more expensive
        // collision check
        if (!zombie.get_global_bounds().findIntersection(
                bullet.get_global_bounds())) {
          ++it;
          continue;
        }

        CollisionDetector detector;
        detector.add_circle(zombie.get_position(), zombie.get_radius());
        if (bullet.get_shape() == BulletShape::CIRCLE) {
          detector.add_circle(bullet.get_position(),
                              bullet.get_global_bounds().size.x);
        } else if (bullet.get_shape() == BulletShape::RECTANGLE) {
          detector.add_rectangle(bullet.get_position(),
                                 bullet.get_global_bounds().size,
                                 bullet.get_rotation());
        } else if (bullet.get_shape() == BulletShape::CONVEX) {
          detector.add_rectangle(bullet.get_position(),
                                 bullet.get_global_bounds().size,
                                 bullet.get_rotation());
        }

        if (detector.collides()) {
          zombie.decrement_health(bullet.get_damage());
          if (zombie.get_health() <= 0.f) {
            player.increment_xp(zombie.get_xp());
            it = zombies.erase(it);

            if (bullet.get_type() == WeaponType::LASER) {
              // If zombie dies to laser, spawn five more lower-damage ones
              for (int i = 0; i < 5; ++i) {
                auto body_shape = std::make_unique<sf::RectangleShape>(
                    sf::Vector2f{20.f, 2.f});
                body_shape->setFillColor(sf::Color::White);
                bullets.emplace_back(
                    bullet.get_position(),
                    bullet.get_velocity().rotatedBy(random_angle(0.f)),
                    WeaponType::LASER, bullet.get_damage() / 10,
                    std::move(body_shape), BulletShape::RECTANGLE);
              }
            } else if (bullet.get_type() == WeaponType::ROCKET_LAUNCHER) {
              // If zombie dies to rocket launcher, deal area damage
              for (auto& area_zombie : zombies) {
                if (std::hypot(
                        area_zombie.get_position().x - bullet.get_position().x,
                        area_zombie.get_position().y -
                            bullet.get_position().y) < 120.f) {
                  area_zombie.decrement_health(bullet.get_damage());
                }
              }

              // Draw explosion radius
              auto body_shape = std::make_unique<sf::CircleShape>(60.f);
              body_shape->setFillColor(sf::Color::Yellow);
              body_shape->setOutlineThickness(36.f);
              body_shape->setOutlineColor(sf::Color::Red);
              bullets.emplace_back(
                  bullet.get_position(), sf::Vector2f{0.f, 0.f},
                  WeaponType::FLAMETHROWER, bullet.get_damage(),
                  std::move(body_shape), BulletShape::CIRCLE);
            }
          }

          bullets.erase(bullets.begin() + i);
          break;
        }

        ++it;
      }

      if (!MAP_BOUNDS.contains(bullet.get_position()) || bullet.expired()) {
        bullets.erase(bullets.begin() + i);
      }
    }

    // Remove zombies killed by collateral damage
    std::erase_if(zombies, [&](const auto& zombie) -> bool {
      if (zombie.get_health() <= 0.f) {
        player.increment_xp(zombie.get_xp());
        return true;
      } else {
        return false;
      }
    });

    // Check for player -> weapon crate collisions
    for (auto it = weapon_crates.begin(); it != weapon_crates.end();) {
      auto& crate = *it;

      // If bounding boxes don't intersect, skip more expensive
      // collision check
      if (!player.get_global_bounds().findIntersection(
              crate.get_global_bounds())) {
        ++it;
        continue;
      }

      CollisionDetector detector;
      detector.add_circle(player.get_position(), player.get_radius());
      detector.add_rectangle(crate.get_position(), crate.get_size(),
                             sf::radians(0.f));

      // If player collided with weapon crate, pick it up
      if (detector.collides()) {
        player.get_weapon(crate.get_type()).ammo += crate.get_ammo();
        player.switch_weapon(crate.get_type());

        it = weapon_crates.erase(it);
        continue;
      }

      // If crate is too old, despawn it
      if (crate.expired()) {
        it = weapon_crates.erase(it);
        continue;
      }

      ++it;
    }

    // Check for zombie -> player collisions
    for (auto& zombie : zombies) {
      // If zombie intersects player, inflict damage to player
      if (std::hypot(zombie.get_position().x - player.get_position().x,
                     zombie.get_position().y - player.get_position().y) <
          player.get_radius() + zombie.get_radius()) {
        player.decrement_health(100.f * frame_duration);
      }
    }

    // Show pause menu or game over screen if applicable
    bool reset_game = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
      if (display_pause_menu(main_window, player.get_position())) {
        reset_game = true;
      }
    }
    if (player.get_health() <= 0.f) {
      game_over(main_window, player.get_xp(), player.get_position());
      display_main_menu(main_window, player.get_position());
      reset_game = true;
    }

    if (reset_game) {
      view.setCenter(SCREEN_DIMS / 2.f);
      main_window.setView(view);
      Zombie::reset();
      WeaponCrate::reset();
      zombies.clear();
      bullets.clear();
      weapon_crates.clear();

      player = Player{SCREEN_DIMS / 2.f};
    }

    main_window.clear(BACKGROUND_COLOR);

    main_window.draw(ground_sprite);

    for (auto& weapon_crate : weapon_crates) {
      weapon_crate.draw(main_window);
    }

    for (auto& zombie : zombies) {
      zombie.draw(main_window);
    }

    player.draw(main_window);

    for (auto& bullet : bullets) {
      bullet.draw(main_window);
    }

    main_window.display();
  }
}
