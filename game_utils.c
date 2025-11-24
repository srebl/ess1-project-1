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
 * @brief Asteroids Check Collision
 *
 * @param[in] game_state           current state of game
 *
 * Checks if player has hit an asteroid
 */
CPU_VOID check_collisions(GameState* game_state) {
    const CPU_FP32 pX = game_state->player.x;
    const CPU_FP32 pY = game_state->player.y;
    const CPU_FP32 pSize = (CPU_FP32)game_state->player.size;
    
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!game_state->asteroids[i].is_active) {
            continue;
        }

        //Cast asteroid size to float for safe addition
        const CPU_FP32 aSize = (CPU_FP32)game_state->asteroids[i].size;
        
        //Axis-Aligned Bounding Box
        if( // Player is NOT to the left of the asteroid's right edge
            pX < game_state->asteroids[i].x + aSize               &&
            // Player's right edge is NOT to the left of the asteroid's left edge
            pX + pSize > game_state->asteroids[i].x               && 
            // Player is NOT above the asteroid's bottom edge
            pY < game_state->asteroids[i].y + aSize               &&             
            // Player's bottom edge is NOT above the asteroid's top edge
            pY + pSize > game_state->asteroids[i].y
           ){
            
            game_state->mode = GAME_MODE_GAME_OVER;
            break;
        }
    }
}

/**
 * @brief Asteroid Movements
 *
 * @param[in] game_state           current state of game
 *
 * Calculates asteroid movements
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
 * @brief Asteroids Spawn Asteroids
 *
 * @param[in] game_state           current state of game
 *
 * Spawns asteroids into game area
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
 * @brief Asteroids Destroy Asteroid
 *
 * @param[in] game_state           current state of game
 *
 * Remove asteroids from game area if it leaves game area
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
 * @brief Asteroids Run Frame
 *
 * @param[in] game_state           current state of game
 *
 * Runs frame for all game objects
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

/* [] END OF FILE */
