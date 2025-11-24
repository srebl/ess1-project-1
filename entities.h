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
  
#define MAX_ASTEROIDS 3
#define MIN_ASTEROID_SIZE 3
#define MAX_ASTEROID_SIZE 20
  
#define MAX_PLAYER_SPEED 8.0f
#define STOPPED_SPEED    0.0f
#define PLAYER_SIZE      7

/*
*********************************************************************************************************
*                                             STRUCTS
*********************************************************************************************************
*/

//Asteroid struct
typedef struct{
  CPU_INT08U size;
  CPU_FP32 x,y,vX,vY;
  CPU_BOOLEAN is_active;
}Asteroid;

//Player state struct
typedef enum {
  PLAYER_STATE_ALIVE,
  PLAYER_STATE_EXPLODING
} PlayerState;

//Player struct
typedef struct {
  CPU_INT08U size;
  CPU_FP32 x,y,vX,vY;
  CPU_INT16U score;
  PlayerState player_state;
  CPU_INT08U* sprite;
} Player;

//GameState Gamemode Struct
typedef enum {
    GAME_MODE_MENU,
    GAME_MODE_RUNNING,
    GAME_MODE_PAUSED,
    GAME_MODE_GAME_OVER
} GameMode;

//GameState struct
typedef struct{
  GameMode mode;
  Asteroid asteroids [MAX_ASTEROIDS];
  Player player;
}GameState;


#endif
/* [] END OF FILE */
