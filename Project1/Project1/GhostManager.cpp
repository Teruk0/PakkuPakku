#include <array>  // For std::array
#include <cmath>  // For mathematical operations like pow
#include <SFML/Graphics.hpp> // For SFML graphics components

#include "Headers/Global.hpp"     // Header for global constants and definitions
#include "Headers/Pacman.hpp"     // Header for Pac-Man class definition
#include "Headers/Ghost.hpp"      // Header for Ghost class definition
#include "Headers/GhostManager.hpp" // Header for GhostManager class definition

// Constructor for the GhostManager class
GhostManager::GhostManager() :
    current_wave(0),  // Initialize the current wave to 0
    wave_timer(LONG_SCATTER_DURATION),  // Initialize the wave timer for the first scatter mode
    ghosts({ Ghost(0), Ghost(1), Ghost(2), Ghost(3) })  // Create four ghosts with unique IDs
{
}

// Draws all the ghosts managed by this GhostManager on the provided SFML render window
void GhostManager::draw(bool i_flash, sf::RenderWindow& i_window) {
    // Loop through all the ghosts and draw each one
    for (Ghost& ghost : ghosts) {
        ghost.draw(i_flash, i_window);  // Draw the ghost with possible flash effect
    }
}

// Reset the GhostManager for a specific level and set the initial positions for ghosts
void GhostManager::reset(
    unsigned char i_level,
    const std::array<Position, 4>& i_ghost_positions
) {
    current_wave = 0;  // Reset the current wave

    // Adjust the wave timer based on the level to increase difficulty
    wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));

    // Set the initial positions for each ghost based on the provided array
    for (unsigned char a = 0; a < 4; a++) {
        ghosts[a].set_position(i_ghost_positions[a].x, i_ghost_positions[a].y);
    }

    // Reset each ghost, using the blue ghost's position for the house and the red ghost's position for the exit
    for (Ghost& ghost : ghosts) {
        ghost.reset(ghosts[2].get_position(), ghosts[0].get_position());
    }
}

// Update the GhostManager and all managed ghosts based on the game level, map, and Pac-Man's state
void GhostManager::update(
    unsigned char i_level,
    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,
    Pacman& i_pacman
) {
    // If Pac-Man's energizer timer is zero (not energized)
    if (i_pacman.get_energizer_timer() == 0) {
        // If the wave timer has reached zero, it's time to switch modes
        if (wave_timer == 0) {
            if (current_wave < 7) {  // Limit the number of waves
                current_wave++;  // Increment the wave count

                // Switch the mode for all ghosts (scatter or chase)
                for (Ghost& ghost : ghosts) {
                    ghost.switch_mode();
                }
            }

            // Determine the new wave timer duration based on the current wave
            if (current_wave % 2 == 1) {
                wave_timer = CHASE_DURATION;  // Set to chase mode duration
            }
            else if (current_wave == 2) {
                wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));  // Adjusted scatter mode duration
            }
            else {
                wave_timer = static_cast<unsigned short>(SHORT_SCATTER_DURATION / pow(2, i_level));  // Shorter scatter mode
            }
        }
        else {
            wave_timer--;  // Decrement the wave timer
        }
    }

    // Update each ghost with the current level, map, and Pac-Man's information
    for (Ghost& ghost : ghosts) {
        ghost.update(i_level, i_map, ghosts[0], i_pacman);  // Update ghost behavior
    }
}
