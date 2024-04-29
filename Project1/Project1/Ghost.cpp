#include <array>  // For std::array
#include <cmath>  // For mathematical operations like sqrt and pow
#include <SFML/Graphics.hpp> // For SFML graphics components

#include "Headers/Global.hpp"     // Header for global constants and definitions
#include "Headers/Pacman.hpp"     // Header for Pac-Man class definition
#include "Headers/Ghost.hpp"      // Header for Ghost class definition
#include "Headers/MapCollision.hpp" // Header for map collision handling

// Constructor for the Ghost class with a unique ID
Ghost::Ghost(unsigned char i_id) :
    id(i_id)  // Initialize the ghost ID
{
    // Fun comment about a common typo
}

// Check if the ghost collides with Pac-Man
bool Ghost::pacman_collision(const Position& i_pacman_position) {
    // Basic collision check: if the ghost is within one CELL_SIZE of Pac-Man in both x and y axes
    return (position.x > i_pacman_position.x - CELL_SIZE &&
        position.x < i_pacman_position.x + CELL_SIZE &&
        position.y > i_pacman_position.y - CELL_SIZE &&
        position.y < i_pacman_position.y + CELL_SIZE);
}

// Get the distance from the ghost to its target in a specific direction
float Ghost::get_target_distance(unsigned char i_direction) {
    // Copy the ghost's current position
    short x = position.x;
    short y = position.y;

    // Adjust the position based on the given direction
    switch (i_direction) {
    case 0: x += GHOST_SPEED; break;  // Right
    case 1: y -= GHOST_SPEED; break;  // Up
    case 2: x -= GHOST_SPEED; break;  // Left
    case 3: y += GHOST_SPEED; break;  // Down
    }

    // Calculate the distance to the target using the Pythagorean theorem
    return static_cast<float>(sqrt(pow(x - target.x, 2) + pow(y - target.y, 2)));
}

// Draw the ghost on the SFML render window, handling animation and frightened states
void Ghost::draw(bool i_flash, sf::RenderWindow& i_window) {
    // Determine the current frame of animation based on the animation timer and speed
    unsigned char body_frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(GHOST_ANIMATION_SPEED)));

    sf::Sprite body;  // Sprite for the ghost's body
    sf::Sprite face;  // Sprite for the ghost's face

    // Load the ghost texture
    sf::Texture texture;
    texture.loadFromFile("Resources/Images/Ghost" + std::to_string(CELL_SIZE) + ".png");

    // Set up the body sprite and its position
    body.setTexture(texture);
    body.setPosition(position.x, position.y);
    // Set the texture rectangle to get the correct frame for animation
    body.setTextureRect(sf::IntRect(CELL_SIZE * body_frame, 0, CELL_SIZE, CELL_SIZE));

    // Set up the face sprite and its position
    face.setTexture(texture);
    face.setPosition(position.x, position.y);

    // Handle the animation and coloring based on the ghost's state
    if (frightened_mode == 0) {  // Not frightened
        // Set the body color based on the ghost's ID (red, pink, cyan, orange)
        switch (id) {
        case 0: body.setColor(sf::Color(255, 0, 0)); break;
        case 1: body.setColor(sf::Color(255, 182, 255)); break;
        case 2: body.setColor(sf::Color(0, 255, 255)); break;
        case 3: body.setColor(sf::Color(255, 182, 85)); break;
        }

        // Set the face's texture rectangle based on the ghost's direction
        face.setTextureRect(sf::IntRect(CELL_SIZE * direction, CELL_SIZE, CELL_SIZE, CELL_SIZE));

        // Draw the body sprite on the window
        i_window.draw(body);

    }
    else if (frightened_mode == 1) {  // Frightened mode
        body.setColor(sf::Color(36, 36, 255)); // Frightened ghosts are blue

        // Set the texture rectangle for the frightened face
        face.setTextureRect(sf::IntRect(4 * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_SIZE));

        // Flash the ghost's body and face when frightened (for flashing state)
        if (i_flash && (body_frame % 2 == 0)) {
            body.setColor(sf::Color(255, 255, 255));
            face.setColor(sf::Color(255, 0, 0));
        }
        else {
            face.setColor(sf::Color(255, 255, 255));
        }

        i_window.draw(body);  // Draw the frightened body

    }
    else {  // If the ghost is fleeing (ghost has been eaten)
        face.setTextureRect(sf::IntRect(CELL_SIZE * direction, 2 * CELL_SIZE, CELL_SIZE, CELL_SIZE));

        i_window.draw(face); // Draw only the face (body is missing)
    }

    // Update the animation timer to create a looping effect for the ghost animation
    animation_timer = (animation_timer + 1) % (GHOST_ANIMATION_FRAMES * GHOST_ANIMATION_SPEED);
}

// Reset the ghost's state to its home position and exit
void Ghost::reset(const Position& i_home, const Position& i_home_exit) {
    movement_mode = 0;  // Set default mode
    use_door = id > 0;  // Only ghosts other than red can use the door

    direction = 0;  // Default direction
    frightened_mode = 0;  // Not frightened
    frightened_speed_timer = 0;  // Reset speed timer

    animation_timer = 0;  // Reset animation timer

    // Set home, home exit, and initial target
    home = i_home;
    home_exit = i_home_exit;
    target = i_home_exit;
}

// Set the ghost's position
void Ghost::set_position(short i_x, short i_y) {
    position = { i_x, i_y };
}

// Toggle between scatter and chase modes
void Ghost::switch_mode() {
    movement_mode = 1 - movement_mode;  // Toggle between scatter and chase
}

// Update the ghost's behavior based on game level, Pac-Man's state, and other factors
void Ghost::update(
    unsigned char i_level,
    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map,
    Ghost& i_ghost_0,
    Pacman& i_pacman
) {
    bool move = false;  // Whether the ghost can move
    unsigned char available_ways = 0;  // Number of available directions to move
    unsigned char speed = GHOST_SPEED;  // Ghost speed

    std::array<bool, 4> walls{};  // Walls around the ghost

    // Handle frightened mode transitions based on Pac-Man's energizer timer
    if (frightened_mode == 0 && i_pacman.get_energizer_timer() == ENERGIZER_DURATION / pow(2, i_level)) {
        frightened_speed_timer = GHOST_FRIGHTENED_SPEED;
        frightened_mode = 1;
    }
    else if (i_pacman.get_energizer_timer() == 0 && frightened_mode == 1) {
        frightened_mode = 0;  // Frightened mode ends
    }

    // Adjust ghost speed for escaping
    if (frightened_mode == 2 &&
        (position.x % GHOST_ESCAPE_SPEED == 0) &&
        (position.y % GHOST_ESCAPE_SPEED == 0)) {
        speed = GHOST_ESCAPE_SPEED;
    }

    // Update the ghost's target based on Pac-Man's direction, other ghosts' positions, and game modes
    update_target(i_pacman.get_direction(), i_ghost_0.get_position(), i_pacman.get_position());

    // Check if the ghost can move in each direction, considering doors and walls
    walls[0] = map_collision(0, use_door, speed + position.x, position.y, i_map);  // Right
    walls[1] = map_collision(0, use_door, position.x, position.y - speed, i_map);  // Up
    walls[2] = map_collision(0, use_door, position.x - speed, position.y, i_map);  // Left
    walls[3] = map_collision(0, use_door, speed + position.y, i_map);  // Down

    if (frightened_mode != 1) {  // Non-frightened logic
        unsigned char optimal_direction = 4;  // Best direction for the ghost

        move = true;  // Ghost can move

        // Check for available directions and choose the optimal one based on target distance
        for (unsigned char a = 0; a < 4; a++) {
            if (a == ((2 + direction) % 4)) continue;  // Prevent turning back unless required

            if (!walls[a]) {
                if (optimal_direction == 4) optimal_direction = a;

                available_ways++;  // Increment available directions

                if (get_target_distance(a) < get_target_distance(optimal_direction)) {
                    optimal_direction = a;  // Choose the optimal direction based on target distance
                }
            }
        }

        // If there are multiple available ways, set the optimal direction
        if (available_ways > 1) {
            direction = optimal_direction;  // Choose the best direction
        }
        else if (optimal_direction == 4) {
            direction = (2 + direction) % 4;  // Turn back if no other way
        }
        else {
            direction = optimal_direction;  // Follow the best path
        }

    }
    else {  // Frightened logic
        unsigned char random_direction = rand() % 4;  // Random direction for frightened ghost

        if (frightened_speed_timer == 0) {
            move = true;  // Ghost can move

            frightened_speed_timer = GHOST_FRIGHTENED_SPEED;  // Reset speed timer

            // Check for available directions without turning back
            for (unsigned char a = 0; a < 4; a++) {
                if (a == ((2 + direction) % 4)) continue;  // Avoid turning back
                if (!walls[a]) {
                    available_ways++;  // Increment available directions
                }
            }

            if (available_ways > 0) {
                // Choose a random direction that is valid and does not turn back
                while (walls[random_direction] || random_direction == ((2 + direction) % 4)) {
                    random_direction = rand() % 4;
                }
                direction = random_direction;
            }
            else {
                // Turn back if no other valid option
                direction = ((2 + direction) % 4);
            }
        }
        else {
            frightened_speed_timer--;  // Decrement speed timer
        }
    }

    // Move the ghost in the determined direction
    if (move) {
        switch (direction) {
        case 0: position.x += speed; break;  // Right
        case 1: position.y -= speed; break;  // Up
        case 2: position.x -= speed; break;  // Left
        case 3: position.y += speed; break;  // Down
        }

        // Handle warp tunnels
        if (position.x < -CELL_SIZE) {
            position.x = CELL_SIZE * MAP_WIDTH - speed;
        }
        else if (position.x >= CELL_SIZE * MAP_WIDTH) {
            position.x = speed - CELL_SIZE;
        }
    }

    // Handle collision with Pac-Man
    if (pacman_collision(i_pacman.get_position())) {
        if (frightened_mode == 0) {  // If ghost is not frightened, it kills Pac-Man
            i_pacman.set_dead(1);
        }
        else {  // If ghost is frightened, it runs towards its home
            use_door = true;  // Allow ghost to use the door
            frightened_mode = 2;  // Set frightened mode to escape
            target = home;  // Target is the ghost's home
        }
    }
}

// Update the ghost's target based on Pac-Man's direction and other game parameters
void Ghost::update_target(
    unsigned char i_pacman_direction,
    const Position& i_ghost_0_position,
    const Position& i_pacman_position
) {
    if (use_door) {  // If the ghost is in escape mode (using the door)
        if (position == target) {
            if (target == home_exit) {  // If ghost has reached the home exit
                use_door = false;  // Cannot use the door anymore
            }
            else if (target == home) {  // If ghost has reached its home
                frightened_mode = 0;  // Reset frightened mode
                target = home_exit;  // Start leaving the house
            }
        }
    }
    else {
        if (movement_mode == 0) {  // Scatter mode
            // Assign targets to ghosts based on their ID
            switch (id) {
            case 0:  // Red ghost target: top-right corner
                target = { CELL_SIZE * (MAP_WIDTH - 1), 0 };
                break;
            case 1:  // Pink ghost target: top-left corner
                target = { 0, 0 };
                break;
            case 2:  // Cyan ghost target: bottom-right corner
                target = { CELL_SIZE * (MAP_WIDTH - 1), CELL_SIZE * (MAP_HEIGHT - 1) };
                break;
            case 3:  // Orange ghost target: bottom-left corner
                target = { 0, CELL_SIZE * (MAP_HEIGHT - 1) };
                break;
            }
        }
        else {  // Chase mode
            switch (id) {
            case 0:  // Red ghost chases Pac-Man directly
                target = i_pacman_position;
                break;

            case 1:  // Pink ghost targets four cells ahead of Pac-Man
                target = i_pacman_position;
                switch (i_pacman_direction) {
                case 0: target.x += CELL_SIZE * GHOST_1_CHASE; break;  // Right
                case 1: target.y -= CELL_SIZE * GHOST_1_CHASE; break;  // Up
                case 2: target.x -= CELL_SIZE * GHOST_1_CHASE; break;  // Left
                case 3: target.y += CELL_SIZE * GHOST_1_CHASE; break;  // Down
                }
                break;

            case 2:  // Cyan ghost targets two cells ahead and moves away from red ghost
                target = i_pacman_position;
                switch (i_pacman_direction) {
                case 0: target.x += CELL_SIZE * GHOST_2_CHASE; break;  // Right
                case 1: target.y -= CELL_SIZE * GHOST_2_CHASE; break;  // Up
                case 2: target.x -= CELL_SIZE * GHOST_2_CHASE; break;  // Left
                case 3: target.y += CELL_SIZE * GHOST_2_CHASE; break;  // Down
                }
                // Double the distance from red ghost
                target.x += target.x - i_ghost_0_position.x;
                target.y += target.y - i_ghost_0_position.y;
                break;

            case 3:  // Orange ghost chases Pac-Man if far away, but switches to scatter when close
                float distance = static_cast<float>(sqrt(pow(position.x - i_pacman_position.x, 2) + pow(position.y - i_pacman_position.y, 2)));
                if (distance > CELL_SIZE * GHOST_3_CHASE) {
                    target = i_pacman_position;
                }
                else {
                    target = { 0, CELL_SIZE * (MAP_HEIGHT - 1) };
                }
                break;
            }
        }
    }
}

// Get the current position of the ghost
Position Ghost::get_position() {
    return position;
}
