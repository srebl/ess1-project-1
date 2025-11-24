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

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <game_utils.h>


/*
*********************************************************************************************************
*                                             FUNCTIONS
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
CPU_VOID check_collisions(GameState* game_state) {
    
    const CPU_FP32 pSize = (CPU_FP32)game_state->player.size;
    if(game_state->player.x < 0 || game_state->player.x > 96 ||
      game_state->player.y < 0 || game_state->player.y > 96){
        game_state->mode = GAME_MODE_GAME_OVER;
        return;
      }
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!game_state->asteroids[i].is_active) {
            continue;
        }

        //Cast asteroid size to float for safe addition
        const CPU_FP32 aSize = (CPU_FP32)game_state->asteroids[i].size;
        
        //Axis-Aligned Bounding Box
        if( // Player is NOT to the left of the asteroid's right edge
            game_state->player.x < game_state->asteroids[i].x + aSize               &&
            // Player's right edge is NOT to the left of the asteroid's left edge
            game_state->player.x + pSize > game_state->asteroids[i].x               && 
            // Player is NOT above the asteroid's bottom edge
            game_state->player.y < game_state->asteroids[i].y + aSize               &&             
            // Player's bottom edge is NOT above the asteroid's top edge
            game_state->player.y + pSize > game_state->asteroids[i].y
           ){
            game_state->mode = GAME_MODE_GAME_OVER;
            break;
        }
    }
}

/**
 * @brief Moves the player and all active asteroids based on their velocities.
 *
 * @param[in] game_state  Game state containing the player and asteroid data.
 *
 * The function increments the player’s position by its velocity, then iterates
 * through the asteroid list and updates the position of each active asteroid.
 * Inactive asteroids are ignored.
 */
CPU_VOID calculate_movement(GameState* game_state){
  game_state->player.x += game_state->player.vX;
  game_state->player.y += game_state->player.vY;
  for(int i = 0; i< MAX_ASTEROIDS; i++){
    if(!game_state->asteroids[i].is_active) continue;
    
    game_state->asteroids[i].x += game_state->asteroids[i].vX;
    game_state->asteroids[i].y += game_state->asteroids[i].vY;
  }
}

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
CPU_VOID spawn_asteroids(GameState* game_state){
    //Define the center target as floats
    CPU_FP32 targetX = (CPU_FP32)game_state->player.x;
    CPU_FP32 targetY = (CPU_FP32)game_state->player.y;
    
    //Define asteroid speed
    CPU_FP32 asteroid_speed = 2.0f;

    for(int i = 0; i < MAX_ASTEROIDS; i++){
        //Find the first inactive asteroid slot
        if(game_state->asteroids[i].is_active) continue;
        
        //Initialize Asteroid
        game_state->asteroids[i].is_active = DEF_TRUE;
        game_state->asteroids[i].size = (MIN_ASTEROID_SIZE) + rand() % (MAX_ASTEROID_SIZE - MIN_ASTEROID_SIZE + 1);
        
        //Cast size to float for calculation
        CPU_FP32 size_f = (CPU_FP32)game_state->asteroids[i].size;
        
        CPU_INT16U spawn_side = rand() % 4; // 0=Top, 1=Bottom, 2=Left, 3=Right
        
        //Temporary float variables for start position
        CPU_FP32 startX = 0.0f;
        CPU_FP32 startY = 0.0f;

        switch (spawn_side) {
            case 0: //Top (Spawn above screen)
                startX = (CPU_FP32)(rand() % GAME_AREA_WIDTH);
                startY = 0.0f - size_f; 
                break;
            case 1: //Bottom (Spawn below screen)
                startX = (CPU_FP32)(rand() % GAME_AREA_WIDTH);
                startY = (CPU_FP32)GAME_AREA_HEIGHT;
                break;
            case 2: //Left (Spawn left of screen)
                startX = 0.0f - size_f;
                startY = (CPU_FP32)(rand() % GAME_AREA_HEIGHT);
                break;
            case 3: //Right (Spawn right of screen)
                startX = (CPU_FP32)GAME_AREA_WIDTH;
                startY = (CPU_FP32)(rand() % GAME_AREA_HEIGHT);
                break;
        }

        //Assign start position
        game_state->asteroids[i].x = startX;
        game_state->asteroids[i].y = startY;

        //Velocity Calculation (Vector Normalization)
        
        //Calculate the vector pointing to the center
        CPU_FP32 dirX = targetX - startX;
        CPU_FP32 dirY = targetY - startY;
        
        //Calculate the length (magnitude) of the vector
        CPU_FP32 length = sqrtf((dirX * dirX) + (dirY * dirY));
        
        //Normalize and multiply by speed
        if(length > 0.0f) {
            game_state->asteroids[i].vX = (dirX / length) * asteroid_speed;
            game_state->asteroids[i].vY = (dirY / length) * asteroid_speed;
        } else {
            //Fallback if length is 0
            game_state->asteroids[i].vX = 0.0f;
            game_state->asteroids[i].vY = 0.0f;
        }
        
        //Spawn only one asteroid per frame refresh
        break; 
    }
}


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
CPU_VOID destroy_asteroids(GameState* game_state){
    for(int i = 0; i< MAX_ASTEROIDS; i++){
        if(!game_state->asteroids[i].is_active) continue;
      
        if((CPU_INT08S)game_state->asteroids[i].x > GAME_AREA_WIDTH + game_state->asteroids[i].size + 3 ||
            (CPU_INT08S)game_state->asteroids[i].x < 0 - game_state->asteroids[i].size - 3.0f){
            game_state->asteroids[i].is_active = DEF_FALSE;
        }
        else if((CPU_INT08S)game_state->asteroids[i].y > GAME_AREA_HEIGHT + game_state->asteroids[i].size + 3 ||
            (CPU_INT08S)game_state->asteroids[i].y < 0 - game_state->asteroids[i].size - 3){
            game_state->asteroids[i].is_active = DEF_FALSE;
        }
    }
}

/**
 * @brief Executes a full update cycle for all game objects.
 * @param[in] game_state  Current state of the game, including object
 *                            positions, velocities, and activity flags.
 *
 * Runs one frame of game logic by performing collision detection, applying
 * movement updates, spawning new asteroids when appropriate, and deactivating
 * asteroids that leave the game area.
 */
CPU_VOID run_a_frame(GameState* game_state){
    check_collisions(game_state);
    calculate_movement(game_state);
    spawn_asteroids(game_state);
    destroy_asteroids(game_state);
}

/**
 * @brief Asteroids Reset
 *
 * @param[in] game_state           current state of game
 *
 * Resets all objects
 */
CPU_VOID reset_asteroids(GameState* game_state){
    for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
        game_state->asteroids[i].is_active = DEF_FALSE;
    }
}

/**
 * @brief Resets the entire game state to initial values.
 * @param[in] game_state  Current state of the game to be reset.
 *
 * Resets the player position, score, and velocity to their default values,
 * and deactivates all asteroids to prepare for a new game session.
 */
CPU_VOID reset_game_state(GameState* game_state){
  game_state->player.x = GAME_AREA_HEIGHT / 2.0f; // Reset player position
  game_state->player.y = GAME_AREA_HEIGHT / 2.0f; // Reset player position
  game_state->player.vX = STOPPED_SPEED; // Stop all movement
  game_state->player.vY = STOPPED_SPEED;
  
  game_state->player.score = 0;
  
   for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
        game_state->asteroids[i].is_active = DEF_FALSE;
    };
}

/**
 * @brief Initializes the game state to default starting values.
 * @param[in] game_state  Pointer to the game state structure to initialize.
 *
 * Sets the player's position to the center of the game area, resets velocity
 * to zero, assigns the default player size, sets the score to zero, and
 * sets the game mode to the menu. All asteroids are marked inactive to start
 * with a clean game field.
 */
CPU_VOID game_state_init(GameState* game_state){
  game_state->player.x = GAME_AREA_HEIGHT / 2.0f; // Reset player position
  game_state->player.y = GAME_AREA_HEIGHT / 2.0f; // Reset player position
  game_state->player.vX = STOPPED_SPEED; // Stop all movement
  game_state->player.vY = STOPPED_SPEED;
  game_state->player.size = PLAYER_SIZE;
  game_state->player.score = 0;
  game_state->mode = GAME_MODE_MENU;
  
  game_state->player.score = 0;
  
   for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
        game_state->asteroids[i].is_active = DEF_FALSE;
    };
}
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
CPU_FP32 compute_velocity(CPU_FP32 input)
{
    const CPU_FP32 dead_low  = CENTER_POINT - (DEAD_ZONE_WIDTH / 2.0f);
    const CPU_FP32 dead_high = CENTER_POINT + (DEAD_ZONE_WIDTH / 2.0f);

    // In the dead zone ⇒ no movement
    if (input >= dead_low && input <= dead_high) {
        return 0.0f;
    }

    // Below center (moving left/up)
    if (input < CENTER_POINT) {
        CPU_FP32 ratio = (input - dead_low) / (dead_low);
        CPU_FP32 v = ratio * MAX_PLAYER_SPEED_F;
        return (v < -MAX_PLAYER_SPEED_F) ? -MAX_PLAYER_SPEED_F : v;
    }

    // Above center (moving right/down)
    CPU_FP32 ratio = (input - dead_high) /
                     (MAX_ACCEL_VALUE - dead_high);

    CPU_FP32 v = ratio * MAX_PLAYER_SPEED_F;
    return (v > MAX_PLAYER_SPEED_F) ? MAX_PLAYER_SPEED_F : v;
}

/* [] END OF FILE */
