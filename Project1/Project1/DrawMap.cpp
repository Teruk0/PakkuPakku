#include <array>  // For std::array
#include <SFML/Graphics.hpp> // For SFML graphics components

#include "Headers/Global.hpp"    // Header for global constants and definitions
#include "Headers/DrawMap.hpp"   // Header for the draw_map function

// Function to draw the game map onto an SFML render window
void draw_map(
    const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,
    sf::RenderWindow& i_window
) {
    // Sprite object for drawing textures on the window
    sf::Sprite sprite;

    // Texture object to load the map texture
    sf::Texture texture;

    // Load the map texture based on a resource file and the defined CELL_SIZE
    texture.loadFromFile("Resources/Images/Map" + std::to_string(CELL_SIZE) + ".png");

    // Set the texture for the sprite
    sprite.setTexture(texture);

    // Iterate over the map's width (columns)
    for (unsigned char a = 0; a < MAP_WIDTH; a++) {
        // Iterate over the map's height (rows)
        for (unsigned char b = 0; b < MAP_HEIGHT; b++) {
            // Set the position of the sprite based on the cell coordinates
            sprite.setPosition(
                static_cast<float>(CELL_SIZE * a),
                static_cast<float>(CELL_SIZE * b)
            );

            // Determine which part of the texture to use based on the cell type
            switch (i_map[a][b]) {
            case Cell::Door:
                // Crop a section of the texture for door cells
                sprite.setTextureRect(
                    sf::IntRect(2 * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_SIZE)
                );
                i_window.draw(sprite); // Draw the sprite to the window
                break;

            case Cell::Energizer:
                // Crop a section of the texture for energizer cells
                sprite.setTextureRect(
                    sf::IntRect(CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_SIZE)
                );
                i_window.draw(sprite); // Draw the sprite to the window
                break;

            case Cell::Pellet:
                // Crop a section of the texture for pellet cells
                sprite.setTextureRect(
                    sf::IntRect(0, CELL_SIZE, CELL_SIZE, CELL_SIZE)
                );
                i_window.draw(sprite); // Draw the sprite to the window
                break;

            case Cell::Wall:
                // Determine neighboring wall connections
                bool down = 0, left = 0, right = 0, up = 0;

                // Check if the cell below is a wall
                if (b < MAP_HEIGHT - 1 && i_map[a][b + 1] == Cell::Wall) {
                    down = 1;
                }

                // Check if the cell to the left is a wall
                if (a > 0 && i_map[a - 1][b] == Cell::Wall) {
                    left = 1;
                }
                else {
                    // If there's a warp tunnel on the left edge
                    left = (a == 0);
                }

                // Check if the cell to the right is a wall
                if (a < MAP_WIDTH - 1 && i_map[a + 1][b] == Cell::Wall) {
                    right = 1;
                }
                else {
                    // If there's a warp tunnel on the right edge
                    right = (a == MAP_WIDTH - 1);
                }

                // Check if the cell above is a wall
                if (b > 0 && i_map[a][b - 1] == Cell::Wall) {
                    up = 1;
                }

                // Calculate the texture rectangle using a unique index for wall connections
                sprite.setTextureRect(
                    sf::IntRect(
                        CELL_SIZE * (down + 2 * (left + 2 * (right + 2 * up))),
                        0,
                        CELL_SIZE,
                        CELL_SIZE
                    )
                );

                // Draw the wall sprite onto the window
                i_window.draw(sprite);
                break;
            }
        }
    }
}
