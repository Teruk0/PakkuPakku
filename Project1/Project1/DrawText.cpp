#include <cmath> // For rounding and math operations
#include <SFML/Graphics.hpp> // For SFML graphics components

#include "Headers/DrawText.hpp" // Header for draw_text function
#include "Headers/Global.hpp"   // Header for global constants and definitions

// Function to draw text onto an SFML render window
void draw_text(
    bool i_center,
    unsigned short i_x,
    unsigned short i_y,
    const std::string& i_text,
    sf::RenderWindow& i_window
) {
    // Variables to keep track of the character's position
    short character_x = i_x;
    short character_y = i_y;

    // Character width to determine the spacing between characters
    unsigned char character_width;

    // SFML sprite to draw individual characters
    sf::Sprite character_sprite;

    // SFML texture to represent the font
    sf::Texture font_texture;

    // Load the font texture from a file
    font_texture.loadFromFile("Resources/Images/Font.png");

    // Determine the width of each character based on the texture's total width
    character_width = font_texture.getSize().x / 96; // The texture contains 96 characters

    // Set the texture for the character sprite
    character_sprite.setTexture(font_texture);

    // If the text needs to be centered horizontally
    if (i_center) {
        // Calculate the initial x position for centered text
        // The expression centers the first line of text within the width of the map
        character_x = static_cast<short>(
            round(0.5f * (CELL_SIZE * MAP_WIDTH - character_width * i_text.substr(0, i_text.find_first_of('\n')).size()))
            );

        // Calculate the initial y position for centered text
        character_y = static_cast<short>(
            round(0.5f * (CELL_SIZE * MAP_HEIGHT - FONT_HEIGHT * (1 + std::count(i_text.begin(), i_text.end(), '\n'))))
            );
    }

    // Loop through each character in the input text
    for (std::string::const_iterator a = i_text.begin(); a != i_text.end(); ++a) {
        if (*a == '\n') {
            // If there's a newline, move to the next line
            if (i_center) {
                // Recalculate the centered x position for the new line
                character_x = static_cast<short>(
                    round(0.5f * (CELL_SIZE * MAP_WIDTH - character_width * i_text.substr(1 + (a - i_text.begin()), i_text.find_first_of('\n', 1 + (a - i_text.begin())) - (1 + (a - i_text.begin()))).size()))
                    );
            }
            else {
                // Otherwise, reset the x position to the initial value
                character_x = i_x;
            }

            // Move down to the next line by adding the font height
            character_y += FONT_HEIGHT;

            continue; // Skip to the next character
        }

        // Set the position of the character sprite
        character_sprite.setPosition(character_x, character_y);

        // Determine the texture rectangle for the character (subtracting 32 to align with the ASCII table)
        character_sprite.setTextureRect(
            sf::IntRect(character_width * (*a - 32), 0, character_width, FONT_HEIGHT)
        );

        // Increment the x position to move to the next character
        character_x += character_width;

        // Draw the character sprite onto the window
        i_window.draw(character_sprite);
    }
}
