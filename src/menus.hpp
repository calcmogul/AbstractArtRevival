// Copyright (c) Tyler Veness

#pragma once

#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

/**
 * Loads high scores from file into list.
 *
 * @param high_score_list High score list.
 */
void load_high_scores(std::vector<unsigned int>& high_score_list);

/**
 * Adds new high score to list, then saves the list to a file.
 *
 * @param high_score_list High score list.
 * @param score New high score.
 */
void save_high_scores(std::vector<unsigned int>& high_score_list,
                      unsigned int score);

/**
 * Displays high scores on main window.
 *
 * @param main_window Main window.
 * @param high_score_list High score list.
 * @param center Center of main window.
 */
void display_high_scores(sf::RenderWindow& main_window,
                         const std::vector<unsigned int>& high_score_list,
                         const sf::Vector2f& center);

/**
 * Displays main menu.
 *
 * @param main_window Main window.
 * @param center Center of main window.
 */
void display_main_menu(sf::RenderWindow& main_window,
                       const sf::Vector2f& center);

/**
 * Displays pause menu.
 *
 * @param main_window Main window.
 * @param center Center of main window.
 * @return True if game should be reset.
 */
bool display_pause_menu(sf::RenderWindow& main_window,
                        const sf::Vector2f& center);

/**
 * Displays game over screen.
 *
 * @param main_window Main window.
 * @param score New high score.
 * @param center Center of main window.
 */
void game_over(sf::RenderWindow& main_window, unsigned int score,
               const sf::Vector2f& center);
