// Copyright (c) Tyler Veness

#pragma once

#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <sleipnir/optimization/problem.hpp>

template <>
inline sf::Vector2<slp::Variable<double>>
sf::Vector2<slp::Variable<double>>::rotatedBy(sf::Angle phi) const {
  // No zero vector assert, because rotating a zero vector is well-defined
  // (yields always itself)
  float cos = std::cos(static_cast<float>(phi.asRadians()));
  float sin = std::sin(static_cast<float>(phi.asRadians()));

  // Don't manipulate x and y separately, otherwise they're overwritten too
  // early
  return Vector2<slp::Variable<double>>(cos * x - sin * y, sin * x + cos * y);
}

class CollisionDetector {
 public:
  /**
   * Constructs a CollisionDetector.
   */
  CollisionDetector() {
    // Finds scaling factor α for which all shapes intersect
    α = problem.decision_variable();
    α.set_value(1.0);
    problem.minimize(α);
    problem.subject_to(α >= 0.0);

    auto x = problem.decision_variable(2);
    point = sf::Vector2<slp::Variable<double>>{x[0], x[1]};
  }

  /**
   * Adds circle object.
   *
   * @param center Circle center.
   * @param radius Circle radius.
   */
  void add_circle(const sf::Vector2f& center, float radius) {
    // Point must be within player circle
    problem.subject_to(slp::pow(point.x - center.x, 2) +
                           slp::pow(point.y - center.y, 2) <=
                       α * radius * radius);

    add_point_to_initial_guess(center);
  }

  /**
   * Adds rectangle object.
   *
   * @param center Rectangle center.
   * @param size Rectangle size.
   * @param rotation Rectangle's clockwise rotation.
   */
  void add_rectangle(const sf::Vector2f& center, const sf::Vector2f& size,
                     sf::Angle rotation) {
    // Rotate point counterclockwise around rectangle center to counteract
    // rectangle's clockwise rotation
    auto point_wrt_rect = (point - sf::Vector2<slp::Variable<double>>{center})
                              .rotatedBy(-rotation);

    // Point must be within rotated rectangle
    problem.subject_to(point_wrt_rect.x >= α * -size.x / 2.f);
    problem.subject_to(point_wrt_rect.x <= α * size.x / 2.f);
    problem.subject_to(point_wrt_rect.y >= α * -size.y / 2.f);
    problem.subject_to(point_wrt_rect.y <= α * size.y / 2.f);

    add_point_to_initial_guess(center);
  }

  /**
   * Returns true if all shapes collide.
   */
  bool collides() {
    point.x.set_value(initial_guess.x);
    point.y.set_value(initial_guess.y);

    // Find scaling factor α for which all shapes intersect
    return problem.solve() == slp::ExitStatus::SUCCESS && α < 1.0;
  }

 private:
  slp::Problem<double> problem;
  slp::Variable<double> α;
  sf::Vector2<slp::Variable<double>> point;

  sf::Vector2f initial_guess{0.f, 0.f};
  int num_shapes = 0;

  /**
   * Add the given point to the running average initial guess.
   *
   * @param point Point to add.
   */
  void add_point_to_initial_guess(const sf::Vector2f& point) {
    ++num_shapes;
    initial_guess = initial_guess * static_cast<float>(num_shapes - 1) /
                        static_cast<float>(num_shapes) +
                    point * 1.f / static_cast<float>(num_shapes);
  }
};
