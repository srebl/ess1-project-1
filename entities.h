/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 *
 * Authors: Lukas Buchmayer, Gavrilo Stanic, Zobay Rahimi, Sebastian Rath
 * 
 * ========================================
*/
#ifndef  ENTITIES_PRESENT
#define  ENTITIES_PRESENT
    
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
    
#include <cpu.h>

/*
*********************************************************************************************************
*                                             DEFINES
*********************************************************************************************************
*/
#define GAME_AREA_HEIGHT 95
#define GAME_AREA_WIDTH 95
  
#define MAX_ASTEROIDS 4
#define MIN_ASTEROID_SIZE 3
#define MAX_ASTEROID_SIZE 15
  
#define STOPPED_SPEED    0.0f
#define PLAYER_SIZE      7
#define CENTER_POINT                                45.0f
#define DEAD_ZONE_WIDTH                             10.0f 
#define MAX_ACCEL_VALUE                             100.0f
#define MAX_PLAYER_SPEED_F                          6.0f  // Float maximum speed
  

/*
*********************************************************************************************************
*                                             STRUCTS
*********************************************************************************************************
*/

/**
 * @struct Asteroid
 * @brief Represents a single asteroid entity in the game world.
 *
 * @details Stores the position, size, movement speed, and active state of an
 *          asteroid. The renderer and game logic use this structure to update
 *          and display each asteroid on the screen.
 *
 * @var Asteroid::size
 *      Pixel size of the asteroid (width/height of its square shape).
 *
 * @var Asteroid::x
 *      X-coordinate of the asteroid in game space.
 *
 * @var Asteroid::y
 *      Y-coordinate of the asteroid in game space.
 *
 * @var Asteroid::vX
 *      Horizontal movement speed.
 *
 * @var Asteroid::vY
 *      Vertical movement speed.
 *
 * @var Asteroid::is_active
 *      Indicates whether this asteroid slot is currently active and should be
 *      updated/rendered.
 */
typedef struct{
  CPU_INT08U size;
  CPU_FP32 x,y,vX,vY;
  CPU_BOOLEAN is_active;
}Asteroid;

/**
 * @enum PlayerState
 * @brief Represents the current lifecycle state of the player.
 *
 * @details Used by game logic and rendering to determine interactions and
 *          visuals (e.g., normal control vs. explosion animation).
 *
 * @var PLAYER_STATE_ALIVE
 *      Player is active and controllable.
 *
 * @var PLAYER_STATE_EXPLODING
 *      Player is in explosion state/animation and not controllable.
 */
typedef enum {
  PLAYER_STATE_ALIVE,
  PLAYER_STATE_EXPLODING
} PlayerState;

/**
 * @struct Player
 * @brief Represents the player entity controlled by accelerometer input.
 *
 * @details Contains position, movement data, and hitbox size for the player.
 *          The renderer and game logic use this to compute collisions and draw
 *          the player on the screen.
 *
 * @var Player::size
 *      Hitbox size of the player in pixels.
 * 
 * @var Player::x
 *      Current X-coordinate of the player.
 *
 * @var Player::y
 *      Current Y-coordinate of the player.
 *
 * @var Player::vX
 *      Horizontal movement speed.
 *
 * @var Player::vY
 *      Vertical movement speed.
 *
 * @var Player::score
 *      Current score of the player
 * 
 * @var Player::player_state
 *      Represents the current lifecycle state of the player.
 * 
 * @var Player::sprite
 *      Place holder for a sprite.
 */
typedef struct {
  CPU_INT08U size;
  CPU_FP32 x,y,vX,vY;
  CPU_INT16U score;
  PlayerState player_state;
  CPU_INT08U* sprite;
} Player;

/**
 * @enum GameMode
 * @brief Defines the current state of the game.
 *
 * @details Used to control the execution flow of all game-related tasks.
 *          Determines whether the gameplay logic, renderer, or end-screen
 *          should be active.
 *
 * @var GAME_MODE_MENU
 *      Game is not running; waiting for start.
 *
 * @var GAME_MODE_RUNNING
 *      Active gameplay state.
 *
 * @var GAME_MODE_GAME_OVER
 *      Game has ended; show game-over state.
 */
typedef enum {
    GAME_MODE_MENU,
    GAME_MODE_RUNNING,
    GAME_MODE_GAME_OVER
} GameMode;

/**
 * @struct GameState
 * @brief Holds the complete state of the game at a given moment.
 *
 * @details Central shared data structure used by multiple RTOS tasks.  
 *          Contains the game mode, player entity, and all asteroids currently
 *          active in the game. Access to this structure must be synchronized
 *          via `gameState_sem`.
 *
 * @var GameState::mode
 *      Current game mode (`GameMode`).
 *
 * @var GameState::asteroids
 *      Array of all asteroid entities in the game (size: MAX_ASTEROIDS).
 *
 * @var GameState::player
 *      The player entity with position and movement data.
 */
typedef struct{
  GameMode mode;
  Asteroid asteroids [MAX_ASTEROIDS];
  Player player;
}GameState;


#endif
/* [] END OF FILE */
