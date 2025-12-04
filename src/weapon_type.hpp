// Copyright (c) Tyler Veness

#pragma once

/// Weapon type.
enum class WeaponType {
  HANDGUN = 0,
  MACHINE_GUN = 1,
  FLAMETHROWER = 2,
  LASER = 3,
  SHOTGUN = 4,
  MINIGUN = 5,
  ROCKET_LAUNCHER = 6
};

constexpr int NUM_WEAPONS = 7;
