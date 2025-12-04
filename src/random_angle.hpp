// Copyright (c) Tyler Veness

#pragma once

#include <numbers>
#include <random>

#include <SFML/System/Angle.hpp>

#include "globals.hpp"

/// Returns random angle within the given tolerance.
///
/// @param accuracy Percent accuracy in the range [0, 1]. A 0 maps to [−π, π]
///     and a 1 maps to [0, 0].
inline sf::Angle random_angle(float accuracy) {
  float angle_error = (1.f - accuracy) * 2.f * std::numbers::pi_v<float>;
  std::uniform_real_distribution<float> distr{-angle_error / 2.f,
                                              angle_error / 2.f};
  return sf::radians(distr(global_engine()));
}
