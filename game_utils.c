/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <game_utils.h>

CPU_BOOLEAN check_collisions(GameState* game_state) {
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (game_state->player.x < game_state->asteroids[i].x + game_state->asteroids[i].size &&
      game_state->player.x + game_state->player.size > game_state->asteroids[i].x && // Use fixed size
      game_state->player.y < game_state->asteroids[i].y + game_state->asteroids[i].size &&
      game_state->player.y + game_state->player.size > game_state->asteroids[i].y) {
      return 1; // Collision detected
    }
  }
  return 0; // No collision
}

CPU_VOID calculate_movement(GameState* game_state){
  game_state->player.x += game_state->player.vX;
  game_state->player.y += game_state->player.vY;
  for(int i = 0; i< MAX_ASTEROIDS; i++){
    if(!game_state->asteroids[i].is_active) continue;
    
    game_state->asteroids[i].x += game_state->asteroids[i].vX;
    game_state->asteroids[i].y += game_state->asteroids[i].vY;
  }
}
/*
CPU_VOID spawn_asteroids(GameState* game_state){
   for(int i = 0; i< MAX_ASTEROIDS; i++){
    if(game_state->asteroids[i].is_active) continue;
    
    game_state->asteroids[i].size = (MIN_ASTEROID_SIZE) + rand() % (MAX_ASTEROID_SIZE - MIN_ASTEROID_SIZE + 1);
    // choose x axis First
    if(rand()%100<50){
      game_state->asteroids[i].x = (0 - game_state->asteroids[i].size) + rand() % ((game_state->asteroids[i].size + GAME_AREA_WIDTH) - (0-game_state->asteroids[i].size) + 1);
      // asteroid will come from up
      if(rand()%100 <50){
        game_state->asteroids[i].y = 0 - game_state->asteroids[i].size;
      }
      // asteroid will come from down
      else{
          game_state->asteroids[i].y = 96 - game_state->asteroids[i].size;
      }
    }
    // choose y axis First
    else{
      game_state->asteroids[i].y = (0 - game_state->asteroids[i].size) + rand() % ((game_state->asteroids[i].size + GAME_AREA_WIDTH) - (0-game_state->asteroids[i].size) + 1);
    
      // asteroid will come from left
      if(rand()%100 <50){
        game_state->asteroids[i].x = 0 - game_state->asteroids[i].size;
      }
      // asteroid will come from right 
      else{
          game_state->asteroids[i].y = 96 - game_state->asteroids[i].size;
      }
    }
    
    //CPU_INT16S vX = game_state->player.x - game_state->asteroids[i].x;
    //CPU_INT16S vY = game_state->player.y - game_state->asteroids[i].y;
    
    CPU_INT16U vX = 45 - game_state->asteroids[i].x;
    CPU_INT16U vY = 45 - game_state->asteroids[i].y;
    CPU_INT16U length = sqrt((vX*vX + vY*vY));
    game_state->asteroids[i].vX = vX/length;
    game_state->asteroids[i].vY = vY/length;
    
    
    game_state->asteroids[0].size = 10;
    game_state->asteroids[0].x = -11;
     game_state->asteroids[0].y = 50;
    game_state->asteroids[0].vX = 1;
    game_state->asteroids[0].vY = 0;
    game_state->asteroids[0].is_active = DEF_TRUE;
    
      game_state->asteroids[1].size = 10;
    game_state->asteroids[1].x = 105;
     game_state->asteroids[1].y = 13;
    game_state->asteroids[1].vX = -2;
    game_state->asteroids[1].vY = 1;
    game_state->asteroids[1].is_active = DEF_TRUE;
    
  }
}
*/

CPU_VOID spawn_asteroids(GameState* game_state){
    for(int i = 0; i < MAX_ASTEROIDS; i++){
        // 1. Find the first inactive asteroid slot
        if(game_state->asteroids[i].is_active) continue;
        
        // --- Asteroid i is inactive, so we will initialize it ---

        game_state->asteroids[i].is_active = DEF_TRUE; // Set it active first
        game_state->asteroids[i].size = (MIN_ASTEROID_SIZE) + rand() % (MAX_ASTEROID_SIZE - MIN_ASTEROID_SIZE + 1);
        
        CPU_INT16U spawn_side = rand() % 4; // 0=Top, 1=Bottom, 2=Left, 3=Right
        
        switch (spawn_side) {
            case 0: // Top
                game_state->asteroids[i].x = rand() % GAME_AREA_WIDTH;
                game_state->asteroids[i].y = 0 - game_state->asteroids[i].size;
                break;
            case 1: // Bottom
                game_state->asteroids[i].x = rand() % GAME_AREA_WIDTH;
                game_state->asteroids[i].y = GAME_AREA_HEIGHT;
                break;
            case 2: // Left
                game_state->asteroids[i].x = 0 - game_state->asteroids[i].size;
                game_state->asteroids[i].y = rand() % GAME_AREA_HEIGHT;
                break;
            case 3: // Right
                game_state->asteroids[i].x = GAME_AREA_WIDTH;
                game_state->asteroids[i].y = rand() % GAME_AREA_HEIGHT;
                break;
        }

        // Calculate velocity to move towards the center (e.g., 45, 48)
        // Ensure you cast to a floating-point type if vX/vY are not integers
        // Using a fixed speed for simplicity if you don't use floating point
        CPU_INT16U targetX = GAME_AREA_WIDTH / 2;
        CPU_INT16U targetY = GAME_AREA_HEIGHT / 2;

        CPU_INT16U vX_raw = targetX - game_state->asteroids[i].x;
        CPU_INT16U vY_raw = targetY - game_state->asteroids[i].y;
        
        // NOTE: This velocity calculation is incomplete without normalization.
        // For simple integer movement, you might just use fixed values like:
           CPU_INT16U vX = 45 - game_state->asteroids[i].x;
        CPU_INT16U vY = 45 - game_state->asteroids[i].y;
        CPU_INT16U length = sqrt((vX*vX + vY*vY));
        game_state->asteroids[i].vX = (vX_raw > 0) ? 1 : -1;
        game_state->asteroids[i].vY = (vY_raw > 0) ? 1 : -1;
        
        // To spawn only one asteroid per frame refresh, use break here:
        break; 
        
        // If you want to allow spawning MAX_ASTEROIDS immediately, remove break
    }
}
CPU_VOID destroy_asteroids(GameState* game_state){
  for(int i = 0; i< MAX_ASTEROIDS; i++){
    if(!game_state->asteroids[i].is_active) continue;
      
      if(game_state->asteroids[i].x > GAME_AREA_WIDTH + game_state->asteroids[i].size + 3 ||
        game_state->asteroids[i].x < 0 - game_state->asteroids[i].size - 3){
          game_state->asteroids[i].is_active = DEF_FALSE;
        }
      else if(game_state->asteroids[i].y > GAME_AREA_HEIGHT + game_state->asteroids[i].size + 3 ||
        game_state->asteroids[i].y < 0 - game_state->asteroids[i].size - 3){
        game_state->asteroids[i].is_active = DEF_FALSE;
      }
  }
}

CPU_VOID run_a_frame(GameState* game_state){
  calculate_movement(game_state);
  spawn_asteroids(game_state);
  destroy_asteroids(game_state);
check_collisions(game_state);
}



/* [] END OF FILE */
