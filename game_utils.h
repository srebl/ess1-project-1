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
 * @brief Asteroids Check Collision
 *
 * @param[in] game_state           current state of game
 *
 * Checks if player has hit an asteroid
 */
CPU_VOID check_collisions(GameState* game_state);

/**
 * @brief Asteroids Spawn Asteroids
 *
 * @param[in] game_state           current state of game
 *
 * Spawns asteroids into game area
 */
CPU_VOID spawn_asteroids(GameState* game_state);

/**
 * @brief Asteroid Movements
 *
 * @param[in] game_state           current state of game
 *
 * Calculates asteroid movements
 */
CPU_VOID calculate_movement(GameState* game_state);

/**
 * @brief Asteroids Destroy Asteroid
 *
 * @param[in] game_state           current state of game
 *
 * Remove asteroids from game area if it leaves game area
 */
CPU_VOID destroy_asteroids(GameState* game_state);

/**
 * @brief Asteroids Run Frame
 *
 * @param[in] game_state           current state of game
 *
 * Runs frame for all game objects
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
 * @brief Game state reset
 *
 * @param[in] game_state           current state of game
 *
 * Resets all objects
 */
CPU_VOID reset_game_state(GameState* game_state);
/**
 * @brief Game state init
 *
 * @param[in] game_state           current state of game
 *
 * Initializes the game state
 */
CPU_VOID game_state_init(GameState* game_state);

CPU_FP32 compute_velocity(CPU_FP32 input);
/* [] END OF FILE */
