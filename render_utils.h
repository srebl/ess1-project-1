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
#include <os.h>
#include <cpu.h>
#include <entities.h>
#include <bsp_oledc.h>

/*
*********************************************************************************************************
*                                             DEFINES
*********************************************************************************************************
*/
typedef struct {
  CPU_INT08S x;
  CPU_INT08S y;
  CPU_INT08U width;
  CPU_INT08U height;
}Rectangle;

/*
*********************************************************************************************************
*                                             FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/**
 * @brief Delete slivers
 *
 * @param[in] oldRect           Struct of asteroid of previous frame
 * @param[in] newRect           Struct of asteroid of current frame
 *
 * Draws an asteroid and dirty-frames the asteroids trail by overdrawing it with data from previous
 * position.
 */
void delete_slivers(Rectangle oldRect, Rectangle newRect);

/* [] END OF FILE */
