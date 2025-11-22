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
#pragma once

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
 * 
 * INITIAL FILES TAKEN FROM MikroElektronika mikroSDK.
 * Copyright (c) 2017, MikroElektonika - http://www.mikroe.com
 *
 * ALTERED FOR PSoC 5LP CY8C5888LT*-LP097
 *
 * Authors: Lukas Buchmayer
 * 
 * ========================================
*/
    
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/    
#include "bsp_oledc_dma.h"
#include <asteroids_bitmaps.h>
#include <fonts.h>
#include "stdint.h"
#include "stdbool.h"
#include <os.h>
#include <math.h>

/**
 * @brief OLED C function
 *
 * @param[in] col           column of pixel
 * @param[in] row           row of pixel
 * @param[in] color         pixel color in rgb565
 *
 * Draws a pixel at a specified coordination with a specfied color
 */
void raster_pixel(CPU_INT08U col, CPU_INT08U row, CPU_INT16U color);


static void raster_character( uint16_t ch );

/**
 * @brief OLED C set font
 *
 * @param[in] font          font type chosen
 * @param[in] color         color of text aka infill in rgb565
 * @param[in] orientation   orientation of the letts
 *
 * Selects a given font from an external text file via bitmapping
 */
void raster_set_font(const CPU_INT08U *font, CPU_INT16U color, CPU_INT08U orientation);

/**
 * @brief OLED C box area
 *
 * @param[in] start_col         beginning column of box
 * @param[in] start_row         beginning row of box
 * @param[in] end_col           end column of box
 * @param[in] end_row           end row of box
 * @param[in] color             pixel colors in rgb565
 *
 * Draws a box of pixels of varying size depending on pixel coordinates
 */
static void raster_box_area(CPU_INT08U start_col, CPU_INT08U start_row, CPU_INT08U end_col, CPU_INT08U end_row, CPU_INT16U color);

/**
 * @brief OLED C draw area
 *
 * @param[in] start_col         beginning column of box
 * @param[in] start_row         beginning row of box
 * @param[in] end_col           end column of box
 * @param[in] end_row           end row of box
 * @param[in] img               image bitmap
 *
 * Draws an image based on a bitmap
 */
void raster_draw_area( CPU_INT08U start_col, CPU_INT08U start_row, CPU_INT08U end_col, CPU_INT08U end_row, const CPU_INT08U *img );

/**
 * @brief OLED C fill screen
 *
 * @param[in] color         color in rgb565
 *
 * Draws a box area over the entire screen in a specified color
 */
void raster_fill_screen(CPU_INT16U color);
/**
 * @brief OLED C fill screen
 *
 * @param[in] color         color in rgb565
 *
 * Draws a box area over the entire screen in a specified color
 */
void raster_rectangle(CPU_INT08U col_off, CPU_INT08U row_off, CPU_INT08U col_end, CPU_INT08U row_end, CPU_INT16U color);
/**
 * @brief OLED C image
 *
 * @param[in] img             image bitmap
 * @param[in] column_off      column offset from left of screen
 * @param[in] row_off         row offset from top of screen
 *
 * Draws image form bitmap
 */
void raster_image(const CPU_INT08U *img, CPU_INT08U column_off, CPU_INT08U row_off);

/**
 * @brief OLED C text
 *
 * @param[in] text         string
 * @param[in] col_off      column offset from left of screen
 * @param[in] row_off      row offset from top of screen
 *
 * Draws a box area over the entire screen in a specified color
 */
void raster_text( CPU_INT08U *text, CPU_INT16U x, CPU_INT16U y );

/**
 * @brief OLED C post init
 *
 * Additional setup, to be called during system init
 */
void raster_OLED_C_PostInit();

/**
 * @brief OLED C line
 *
 * @param[in] x1         start x coordinate
 * @param[in] y1         start y coordinate
 * @param[in] x2         end x coordinate
 * @param[in] y2         end y coordinate
 * @param[in] color      string
 *
 * Draws a 1 pixel thick line from x1y1 to x2y2 if x1==x2 or y1==y2
 */
void raster__line(CPU_INT08U x1, CPU_INT08U y1, CPU_INT08U x2, CPU_INT08U y2, CPU_INT16U color);

void raster__line_any(CPU_INT08U x1, CPU_INT08U y1, CPU_INT08U x2, CPU_INT08U y2, CPU_INT16U color);

/**
 * @brief Asteroids draw border
 *
 * Draw 1pixel wide border around entire screen in COLOR_GREEN
 */
void raster_asteroids_DrawBorder();

/**
 * @brief Asteroids game welcome screen
 *
 * First screen you see when "booting up" the game, only shows during first boot
 */
void raster_asteroids_DrawPreGame();

/**
 * @brief Asteroids arena
 *
 * Draws the main component of the game arena/area without objects
 */
void raster_asteroids_DrawArena();

/**
 * @brief Asteroids game over
 *
 * If game collision ooccured, this screen gets posted
 * Game over screen for game, and asks for button click to restart game
 */
void raster_asteroids_DrawGameOver();


/* [] END OF FILE */
