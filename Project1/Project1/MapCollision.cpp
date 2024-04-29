#include <array>  // For std::array
#include <cmath>  // For mathematical operations like floor and ceil

#include "Headers/Global.hpp"      // Header for global constants and definitions
#include "Headers/MapCollision.hpp" // Header for map_collision function definition

// Function to check for collisions or collectables on the map
bool map_collision(
    bool i_collect_pellets,  // Whether to collect pellets and energizers
    bool i_use_door,         // Whether to consider doors as obstacles
    short i_x,               // X-coordinate of the point to check
    short i_y,               // Y-coordinate of the point to check
    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map // The map to check against
) {
    bool output = false;  // Collision result (default to no collision)

    // Calculate the cell coordinates from the provided x and y values
    float cell_x = i_x / static_cast<float>(CELL_SIZE);
    float cell_y = i_y / static_cast<float>(CELL_SIZE);

    // A point can intersect up to four cells (top-left, top-right, bottom-left, bottom-right)
    for (unsigned char a = 0; a < 4; a++) {
        short x = 0;  // x-coordinate of the current cell
        short y = 0;  // y-coordinate of the current cell

        // Determine the specific cell coordinates to check based on the index
        switch (a) {
        case 0:  // Top-left cell
            x = static_cast<short>(floor(cell_x));
            y = static_cast<short>(floor(cell_y));
            break;

        case 1:  // Top-right cell
            x = static_cast<short>(ceil(cell_x));
            y = static_cast<short>(floor(cell_y));
            break;

        case 2:  // Bottom-left cell
            x = static_cast<short>(floor(cell_x));
            y = static_cast<short>(ceil(cell_y));
            break;

        case 3:  // Bottom-right cell
            x = static_cast<short>(ceil(cell_x));
            y = static_cast<short>(ceil(cell_y));
            break;
        }

        // Check if the cell is within the bounds of the map
        if (x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT) {
            // If we're not collecting pellets, check for collisions with walls or doors
            if (!i_collect_pellets) {
                if (i_map[x][y] == Cell::Wall) {  // Collision with a wall
                    output = true;
                }
                else if (!i_use_door && i_map[x][y] == Cell::Door) {  // Collision with a door
                    output = true;
                }
            }
            else {  // If we're collecting pellets and energizers
                if (i_map[x][y] == Cell::Energizer) {  // Found an energizer
                    output = true;  // Collision with collectable
                    i_map[x][y] = Cell::Empty;  // Remove the energizer
                }
                else if (i_map[x][y] == Cell::Pellet) {  // Found a pellet
                    i_map[x][y] = Cell::Empty;  // Remove the pellet
                }
            }
        }
    }

    // Return whether a collision occurred or not
    return output;
}
