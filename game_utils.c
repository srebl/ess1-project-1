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
    if (
      game_state->player.x < game_state->asteroids[i].x + game_state->asteroids[i].size &&
      game_state->player.x + game_state->player.size > game_state->asteroids[i].x &&
      game_state->player.y < game_state->asteroids[i].y + game_state->asteroids[i].size &&
      game_state->player.y + game_state->player.size > game_state->asteroids[i].y
    ) {
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

CPU_VOID spawn_asteroids(GameState* game_state){
   for(int i = 0; i< MAX_ASTEROIDS; i++){
    if(!game_state->asteroids[i].is_active) continue;
    
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
    
    CPU_INT16U vX = game_state->player.x - game_state->asteroids[i].x;
    CPU_INT16U vY = game_state->player.y - game_state->asteroids[i].y;
    
    CPU_INT16U length = sqrt((vX*vX + vY*vY));
    game_state->asteroids[i].vX = vX/length;
    game_state->asteroids[i].vY = vY/length;
    
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



/* [] END OF FILE */
