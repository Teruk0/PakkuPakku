#include <array>  // For std::array
#include <string> // For std::string
#include <SFML/Graphics.hpp> // SFML library for graphics rendering

#include "Headers/Global.hpp"        // Header for global definitions and constants
#include "Headers/Pacman.hpp"        // Header for Pac-Man class definition
#include "Headers/ConvertSketch.hpp" // Header for the convert_sketch function definition

// Function to convert a textual map sketch to a structured game map
std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> convert_sketch(
    const std::array<std::string, MAP_HEIGHT>& i_map_sketch,
    std::array<Position, 4>& i_ghost_positions,
    Pacman& i_pacman
) {
    // Initialize the output map with default empty cells
    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> output_map{};

    // Iterate over the rows of the sketch
    for (unsigned char a = 0; a < MAP_HEIGHT; a++) {
        // Iterate over the columns of the sketch
        for (unsigned char b = 0; b < MAP_WIDTH; b++) {
            // Default cell type is empty
            output_map[b][a] = Cell::Empty;

            // Switch on the character at the current position
            switch (i_map_sketch[a][b]) {
                // Wall cell, representing an obstacle
            case '#':
                output_map[b][a] = Cell::Wall;
                break;

                // Door cell, typically used for ghost exits
            case '=':
                output_map[b][a] = Cell::Door;
                break;

                // Pellet cell, representing food for Pac-Man
            case '.':
                output_map[b][a] = Cell::Pellet;
                break;

                // Position for the red ghost (ghost ID 0)
            case '0':
                i_ghost_positions[0].x = CELL_SIZE * b;
                i_ghost_positions[0].y = CELL_SIZE * a;
                break;

                // Position for the pink ghost (ghost ID 1)
            case '1':
                i_ghost_positions[1].x = CELL_SIZE * b;
                i_ghost_positions[1].y = CELL_SIZE * a;
                break;

                // Position for the cyan ghost (ghost ID 2)
            case '2':
                i_ghost_positions[2].x = CELL_SIZE * b;
                i_ghost_positions[2].y = CELL_SIZE * a;
                break;

                // Position for the orange ghost (ghost ID 3)
            case '3':
                i_ghost_positions[3].x = CELL_SIZE * b;
                i_ghost_positions[3].y = CELL_SIZE * a;
                break;

                // Pac-Man's initial position
            case 'P':
                i_pacman.set_position(CELL_SIZE * b, CELL_SIZE * a);
                break;

                // Energizer cell, representing a power-up
            case 'o':
                output_map[b][a] = Cell::Energizer;
                break;

                // Default case, no special handling required
            default:
                break;
            }
        }
    }

    // Return the structured output map
    return output_map;
}
