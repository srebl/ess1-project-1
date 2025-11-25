/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * Authors: Lukas Buchmayer, Gavrilo Stanic, Zobay Rahimi, Sebastian Rath
 *
 * ========================================
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <cpu.h>
#include<entities.h>
#include<stdlib.h>
#include<os.h>
#include<math.h>

/*
*********************************************************************************************************
*                                             FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/**
 * @brief Detects player–asteroid collisions using AABB overlap checks.
 *
 *
 * @param[in] game_state  Pointer to the current game state structure.
 *
 * Iterates through all active asteroids and performs a 2D axis-aligned
 * bounding box (AABB) intersection test between the player and each asteroid.
 * If any overlap is detected, the game state is updated to GAME_MODE_GAME_OVER.
 */
CPU_VOID check_collisions(GameState* game_state);

/**
 * @brief Spawns a single asteroid at a random edge of the game area.
 * @param[in] game_state  Current game state used for determining the
 *                            spawn position and the player's location.
 *
 * Finds the first inactive asteroid slot and initializes it by assigning a
 * random size and randomly selecting one of the four screen edges as a
 * spawn point. The asteroid is positioned just outside the visible game area.
 *
 * Its velocity is computed by normalizing the vector from the spawn position
 * toward the player, ensuring that newly spawned asteroids move directly
 * toward the player's current location. Only one asteroid is spawned per
 * function call.
 */
CPU_VOID spawn_asteroids(GameState* game_state);

/**
 * @brief Moves the player and all active asteroids based on their velocities.
 *
 * @param[in] game_state  Game state containing the player and asteroid data.
 *
 * The function increments the player’s position by its velocity, then iterates
 * through the asteroid list and updates the position of each active asteroid.
 * Inactive asteroids are ignored.
 */
CPU_VOID calculate_movement(GameState* game_state);

/**
 * @brief Deactivates asteroids that move outside the game area.
 * @param[in] game_state  Current state of the game, including asteroid
 *                            positions and activity flags.
 *
 * Iterates through all asteroids and checks whether each active asteroid has
 * moved beyond the bounds of the game area. If an asteroid's position exceeds
 * the allowed region (with a small margin based on its size), it is marked
 * inactive and removed from gameplay.
 */
CPU_VOID destroy_asteroids(GameState* game_state);

/**
 * @brief Executes a full update cycle for all game objects.
 * @param[in] game_state  Current state of the game, including object
 *                            positions, velocities, and activity flags.
 *
 * Runs one frame of game logic by performing collision detection, applying
 * movement updates, spawning new asteroids when appropriate, and deactivating
 * asteroids that leave the game area.
 */
CPU_VOID run_a_frame(GameState* game_state);

/**
 * @brief Asteroids Reset
 *
 * @param[in] game_state           current state of game
 *
 * Resets all objects
 */
CPU_VOID reset_asteroids(GameState* game_state);
/**
 * @brief Resets the entire game state to initial values.
 * @param[in] game_state  Current state of the game to be reset.
 *
 * Resets the player position, score, and velocity to their default values,
 * and deactivates all asteroids to prepare for a new game session.
 */
CPU_VOID reset_game_state(GameState* game_state);
/**
 * @brief Initializes the game state to default starting values.
 * @param[in] game_state  Pointer to the game state structure to initialize.
 *
 * Sets the player's position to the center of the game area, resets velocity
 * to zero, assigns the default player size, sets the score to zero, and
 * sets the game mode to the menu. All asteroids are marked inactive to start
 * with a clean game field.
 */
CPU_VOID game_state_init(GameState* game_state);
/**
 * @brief Computes player velocity based on gyroscope input.
 * @param[in] input  Raw accelerometer/gyroscope value for one axis.
 *
 * Converts a raw input reading into a velocity value for the player,
 * applying a dead zone around the center to prevent unintentional movement.
 * The velocity is normalized proportionally to the distance from the dead
 * zone and clamped to the maximum allowed speed.
 *
 * @return Velocity value along the axis, in units consistent with game coordinates.
 */
CPU_FP32 compute_velocity(CPU_FP32 input);
/* [] END OF FILE */
