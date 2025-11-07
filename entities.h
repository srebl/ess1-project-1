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
#ifndef  ENTITIES_PRESENT
#define  ENTITIES_PRESENT

#define MAX_ASTEROIDS 4
  
#include <cpu.h>

typedef struct{
  CPU_INT08U x,y,size;
  CPU_INT08S vX,vY;
  CPU_BOOLEAN is_active;
}Asteroid;

typedef enum {
  PLAYER_STATE_ALIVE,
  PLAYER_STATE_EXPLODING
} PlayerState;

typedef struct {
  CPU_INT08U x,y;
  CPU_INT08S vX,vY;
  CPU_INT16U score;
  PlayerState player_state;
  CPU_INT08U* sprite;
} Player;

typedef enum {
    GAME_MODE_MENU,
    GAME_MODE_RUNNING,
    GAME_MODE_PAUSED,
    GAME_MODE_GAME_OVER
} GameMode;

typedef struct{
  GameMode mode;
  Asteroid asteroids [MAX_ASTEROIDS];
  
  Player player;
}GameState;


  
#endif
/* [] END OF FILE */
