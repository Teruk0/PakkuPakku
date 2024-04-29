#include <array>  // For std::array
#include <cmath>  // For mathematical operations like floor and ceil
#include <SFML/Graphics.hpp> // For SFML graphics components

#include "Headers/Global.hpp"      // Header for global constants and definitions
#include "Headers/Pacman.hpp"      // Header for Pac-Man class definition
#include "Headers/MapCollision.hpp" // Header for map collision handling

// Constructor for the Pacman class with default initialization
Pacman::Pacman() :
    animation_over(0),  // Animation hasn't ended yet
    dead(0),            // Pac-Man is not dead initially
    direction(0),       // Default direction (right)
    energizer_timer(0), // No energizer effect initially
    position({ 0, 0 })    // Default position
{
    // Note about avoiding repetition in the code comments
}

// Check if Pac-Man's death animation has finished
bool Pacman::get_animation_over() {
    return animation_over;
}

// Check if Pac-Man is dead
bool Pacman::get_dead() {
    return dead;
}

// Get the current direction Pac-Man is facing
unsigned char Pacman::get_direction() {
    return direction;
}

// Get the current energizer timer
unsigned short Pacman::get_energizer_timer() {
    return energizer_timer;
}

// Draw Pac-Man on the SFML render window
void Pacman::draw(bool i_victory, sf::RenderWindow& i_window) {
    unsigned char frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(PACMAN_ANIMATION_SPEED)));

    sf::Sprite sprite;  // Sprite to draw Pac-Man
    sf::Texture texture; // Texture for Pac-Man's sprites

    sprite.setPosition(position.x, position.y);  // Set the sprite's position

    // If Pac-Man is dead or there's a victory animation to play
    if (dead || i_victory) {
        // If the death animation is still playing
        if (animation_timer < PACMAN_DEATH_FRAMES * PACMAN_ANIMATION_SPEED) {
            animation_timer++;  // Increment the animation timer

            // Load the texture for Pac-Man's death animation
            texture.loadFromFile("Resources/Images/PacmanDeath" + std::to_string(CELL_SIZE) + ".png");

            sprite.setTexture(texture);  // Set the sprite's texture
            sprite.setTextureRect(sf::IntRect(CELL_SIZE * frame, 0, CELL_SIZE, CELL_SIZE));  // Set the frame to draw

            i_window.draw(sprite);  // Draw the sprite on the window
        }
        else {
            // Animation is over
            animation_over = 1;
        }
    }
    else {  // Normal animation when Pac-Man is alive
        texture.loadFromFile("Resources/Images/Pacman" + std::to_string(CELL_SIZE) + ".png");

        sprite.setTexture(texture);  // Set the sprite's texture
        sprite.setTextureRect(sf::IntRect(CELL_SIZE * frame, CELL_SIZE * direction, CELL_SIZE, CELL_SIZE));  // Set the frame

        i_window.draw(sprite);  // Draw the sprite

        // Loop the animation
        animation_timer = (animation_timer + 1) % (PACMAN_ANIMATION_FRAMES * PACMAN_ANIMATION_SPEED);
    }
}

// Reset Pac-Man's state to the default values
void Pacman::reset() {
    animation_over = 0;  // Reset animation over status
    dead = 0;  // Reset dead status
    direction = 0;  // Default direction
    animation_timer = 0;  // Reset animation timer
    energizer_timer = 0;  // Reset energizer timer
}

// Set the animation timer for Pac-Man
void Pacman::set_animation_timer(unsigned short i_animation_timer) {
    animation_timer = i_animation_timer;
}

// Set the dead status for Pac-Man
void Pacman::set_dead(bool i_dead) {
    dead = i_dead;

    if (dead) {  // If Pac-Man is dead, reset the animation timer
        animation_timer = 0;
    }
}

// Set Pac-Man's position on the game map
void Pacman::set_position(short i_x, short i_y) {
    position = { i_x, i_y };  // Set the position
}

// Update Pac-Man's state and movement based on keyboard input and map collisions
void Pacman::update(
    unsigned char i_level,
    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map
) {
    // Detect collisions with walls in all four directions
    std::array<bool, 4> walls{};
    walls[0] = map_collision(0, 0, PACMAN_SPEED + position.x, position.y, i_map);  // Right
    walls[1] = map_collision(0, 0, position.x, position.y - PACMAN_SPEED, i_map);  // Up
    walls[2] = map_collision(0, 0, position.x - PACMAN_SPEED, position.y, i_map);  // Left
    walls[3] = map_collision(0, 0, position.x, position.y + PACMAN_SPEED, i_map);  // Down

    // Change direction based on keyboard input and walls
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !walls[0]) {
        direction = 0;  // Right
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !walls[1]) {
        direction = 1;  // Up
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !walls[2]) {
        direction = 2;  // Left
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !walls[3]) {
        direction = 3;  // Down
    }

    // Move Pac-Man in the chosen direction if there's no wall
    if (!walls[direction]) {
        switch (direction) {
        case 0: position.x += PACMAN_SPEED; break;  // Right
        case 1: position.y -= PACMAN_SPEED; break;  // Up
        case 2: position.x -= PACMAN_SPEED; break;  // Left
        case 3: position.y += PACMAN_SPEED; break;  // Down
        }
    }

    // Handle wrap-around if Pac-Man goes beyond the map bounds
    if (position.x < -CELL_SIZE) {
        position.x = CELL_SIZE * MAP_WIDTH - PACMAN_SPEED;
    }
    else if (position.x >= CELL_SIZE * MAP_WIDTH) {
        position.x = PACMAN_SPEED - CELL_SIZE;
    }

    // Check for collisions with pellets or energizers and update the energizer timer
    if (map_collision(1, 0, position.x, position.y, i_map)) {
        energizer_timer = static_cast<unsigned short>(ENERGIZER_DURATION / pow(2, i_level));  // Reset energizer timer
    }
    else {
        energizer_timer = std::max(0, energizer_timer - 1);  // Decrease the energizer timer
    }
}

// Get Pac-Man's current position
Position Pacman::get_position() {
    return position;  // Return Pac-Man's position
}
