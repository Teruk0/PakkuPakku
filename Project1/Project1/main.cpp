#include <array>  // For the std::array class template
#include <chrono> // For time handling
#include <ctime>  // For generating random seeds
#include <SFML/Graphics.hpp> // SFML graphics library

#include "Headers/Global.hpp"        // Custom global header file
#include "Headers/DrawText.hpp"      // Header for drawing text on screen
#include "Headers/Pacman.hpp"        // Header for Pac-Man class definition
#include "Headers/Ghost.hpp"         // Header for Ghost class definition
#include "Headers/GhostManager.hpp"  // Header for managing ghosts
#include "Headers/ConvertSketch.hpp" // Header for converting map sketch to a game map
#include "Headers/DrawMap.hpp"       // Header for drawing the game map
#include "Headers/MapCollision.hpp"  // Header for handling collisions in the map

int main() {
    // Tracks whether the game is won or not
    bool game_won = 0;

    // Used to track time-based lag for framerate independence
    unsigned lag = 0;

    // Level variable to track the current level
    unsigned char level = 0;

    // Time point to measure elapsed time for game logic
    std::chrono::time_point<std::chrono::steady_clock> previous_time;

    // Initial game map represented as a sketch (a grid of characters)
    std::array<std::string, MAP_HEIGHT> map_sketch = {
        " ################### ",
        " #........#........# ",
        " #o##.###.#.###.##o# ",
        " #.................# ",
        " #.##.#.#####.#.##.# ",
        " #....#...#...#....# ",
        " ####.### # ###.#### ",
        "    #.#   0   #.#    ",
        "#####.# ##=## #.#####",
        "     .  #123#  .     ",
        "#####.# ##### #.#####",
        "    #.#       #.#    ",
        " ####.# ##### #.#### ",
        " #........#........# ",
        " #.##.###.#.###.##.# ",
        " #o.#.....P.....#.o# ",
        " ##.#.#.#####.#.#.## ",
        " #....#...#...#....# ",
        " #.######.#.######.# ",
        " #.................# ",
        " ################### "
    };

    // 2D array representing the game map, filled later
    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map{};

    // Stores initial ghost positions
    std::array<Position, 4> ghost_positions;

    // SFML event object to handle game events
    sf::Event event;

    // Create a render window for the game with a specific size and style
    sf::RenderWindow window(
        sf::VideoMode(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE,
            (FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE),
        "Pac-Man",
        sf::Style::Close
    );

    // Set the view to fit the window size
    window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * MAP_WIDTH,
        FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)));

    // Instantiate the ghost manager and Pac-Man
    GhostManager ghost_manager;
    Pacman pacman;

    // Seed the random number generator with the current time for randomness
    srand(static_cast<unsigned>(time(0)));

    // Convert the sketch into a structured map and set initial ghost and Pac-Man positions
    map = convert_sketch(map_sketch, ghost_positions, pacman);

    // Reset the ghost manager for the current level and set initial positions
    ghost_manager.reset(level, ghost_positions);

    // Store the initial time for measuring frame lag
    previous_time = std::chrono::steady_clock::now();

    // Game loop runs while the window is open
    while (window.isOpen()) {
        // Calculate elapsed time since the last frame
        unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - previous_time
            ).count();

        // Add the elapsed time to the lag tracker
        lag += delta_time;

        // Update the previous time for the next frame calculation
        previous_time += std::chrono::microseconds(delta_time);

        // While lag exceeds or is equal to the defined frame duration
        while (FRAME_DURATION <= lag) {
            // Decrease lag by one frame duration to keep the game running smoothly
            lag -= FRAME_DURATION;

            // Poll all SFML events in the queue
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    // If the window close event is triggered, close the game window
                    window.close();
                }
            }

            if (!game_won && !pacman.get_dead()) {
                // Set game_won to 1 temporarily (check if any pellets are left)
                game_won = 1;

                // Update Pac-Man's state
                pacman.update(level, map);

                // Update ghost behavior
                ghost_manager.update(level, map, pacman);

                // Check all cells in the map to see if any pellets are left
                for (const std::array<Cell, MAP_HEIGHT>& column : map) {
                    for (const Cell& cell : column) {
                        if (cell == Cell::Pellet) {
                            // If at least one pellet is found, set game_won to 0 (game not over)
                            game_won = 0;
                            break;
                        }
                    }
                    if (!game_won) break;
                }

                // If all pellets are collected, prepare for level transition
                if (game_won) {
                    pacman.set_animation_timer(0);
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                // If the Enter key is pressed, restart the game logic

                game_won = 0; // Reset game_won flag

                if (pacman.get_dead()) {
                    level = 0; // Reset to level 0 if Pac-Man died
                }
                else {
                    // Increment level if Pac-Man won
                    level++;
                }

                // Reset the map and ghost manager for the new level
                map = convert_sketch(map_sketch, ghost_positions, pacman);
                ghost_manager.reset(level, ghost_positions);

                pacman.reset(); // Reset Pac-Man's state
            }

            if (FRAME_DURATION > lag) {
                // If there's still lag, redraw the game graphics

                window.clear(); // Clear the window for redrawing

                if (!game_won && !pacman.get_dead()) {
                    // Draw the game map
                    draw_map(map, window);

                    // Draw ghosts, with a check for flashing state (ghosts are vulnerable)
                    ghost_manager.draw(GHOST_FLASH_START >= pacman.get_energizer_timer(), window);

                    // Display the current level on the screen
                    draw_text(0, 0, CELL_SIZE * MAP_HEIGHT, "Level: " + std::to_string(1 + level), window);
                }

                // Draw Pac-Man with the game status
                pacman.draw(game_won, window);

                if (pacman.get_animation_over()) {
                    if (game_won) {
                        // If the game is won, display "Next level!"
                        draw_text(1, 0, 0, "Next level!", window);
                    }
                    else {
                        // If Pac-Man died, display "Game over"
                        draw_text(1, 0, 0, "Game over", window);
                    }
                }

                // Show the drawn graphics on the screen
                window.display();
            }
        }
    }
}
