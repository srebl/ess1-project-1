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

#include <os.h>
#include <cpu.h>
#include <entities.h>
#include <bsp_oledc.h>

typedef struct {
  CPU_INT08S x;
  CPU_INT08S y;
  CPU_INT08U width;
  CPU_INT08U height;
}Rectangle;


void delete_slivers(Rectangle oldRect, Rectangle newRect);

/* [] END OF FILE */
