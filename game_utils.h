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
#include <cpu.h>
#include<entities.h>
#include<stdlib.h>
#include<os.h>
#include<math.h>

CPU_BOOLEAN check_collisions(GameState* game_state);
CPU_VOID spawn_asteroids(GameState* game_state);
CPU_VOID calculate_movement(GameState* game_state);
CPU_VOID destroy_asteroids(GameState* game_state);
/* [] END OF FILE */
