# Abstract Art Revival

A zombie survival game rendered with basic shapes.

| Action                    | Keybinding      |
|---------------------------|-----------------|
| Move Up                   | W/Arrow Up      |
| Move Left                 | A/Arrow Left    |
| Move Down                 | S/Arrow Down    |
| Move Right                | D/Arrow Right   |
| Sprint                    | Spacebar (Hold) |
| Fire Weapon               | LMB             |
| Switch To Previous Weapon | Q               |
| Switch To Next Weapon     | E               |
| Pause                     | Escape          |

## HUD

* Your character is the color wheel in the center of the window.
* Your health is represented by how filled in the color wheel is.
* Your stamina is displayed as a blue dial around the color wheel.
* Your currently equipped weapon and its ammunition is displayed to the right of the color wheel.

## Combat

Aim the mouse at enemies and press the left mouse button to fire. Enemies occupying your space will deal damage to you. When your health is fully depleted, the game is over.

## Weapons

| Type            | Ammunition | Damage | Accuracy | Notes                                                    |
|-----------------|------------|--------|----------|----------------------------------------------------------|
| Handgun         | 1,000      | 200    | 100%     | Ammunition not available in weapon crates                |
| Machine gun     | 250        | 50     | 98%      | Rapid-fire weapon                                        |
| Flamethrower    | 200        | 200    | 90%      | Flame expands as it goes                                 |
| Laser           | 10         | 2,000  | 100%     | Impact spawns 5 lower-damage lasers in random directions |
| Shotgun         | 20         | 75     | 90%      | Shoots multiple rounds in a spread                       |
| Minigun         | 500        | 100    | 90%      | Fastest rate of fire                                     |
| Rocket launcher | 10         | 2,000  | 100%     | Impact deals area damage                                 |

Replenish ammunition by picking up weapon crates (brown squares).
