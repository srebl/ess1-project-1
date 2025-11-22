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

#ifndef  BSP_OLED_MODULE_PRESENT
#define  BSP_OLED_MODULE_PRESENT

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "stdint.h"
#include "stdbool.h"
#include <os.h>
#include <SPIM_1.h>
#include <OLED_C_Enable.h>
#include <OLED_C_DC.h>
#include <OLED_C_Reset.h>
#include <OLED_C_CS.h>
#include <fonts.h>
#include <entities.h>
#include <string.h>
#include <stdlib.h>
    
/*
*********************************************************************************************************
*                                             COLORS
*********************************************************************************************************
*/
#define COLOR_GREEN     0x3fe7
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xf8c3
#define COLOR_BLACK     0x0000
    
/*
*********************************************************************************************************
*                                             DEFINES
*********************************************************************************************************
*/    
#define T_OLEDC_P const CPU_INT08U*

#define OLEDC_SCREENSIZE 96
#define OLEDC_BUBBLESIZE 24

#define OLEDC_SCREENHEIGHT 95

#define __OLEDC_DRV_SPI__
    
#define OLEDC_TOTAL_PIXELS 9216
#define OLEDC_FRAME_BUFFER_SIZE (OLEDC_TOTAL_PIXELS * 2)
extern CPU_INT08U oled_ram_buffer[OLEDC_FRAME_BUFFER_SIZE];

/*
*********************************************************************************************************
*                                             FONT ORIENTATIONS
*********************************************************************************************************
*/
extern const CPU_INT08U _OLEDC_FO_HORIZONTAL;
extern const CPU_INT08U  _OLEDC_FO_VERTICAL;
extern const CPU_INT08U  _OLEDC_FO_VERTICAL_COLUMN;

/*
*********************************************************************************************************
*                                             REMAMP
*********************************************************************************************************
*/
extern const CPU_INT08U _OLEDC_RMP_INC_HOR;
extern const CPU_INT08U _OLEDC_RMP_INC_VER;
extern const CPU_INT08U _OLEDC_RMP_COLOR_NOR;
extern const CPU_INT08U _OLEDC_RMP_COLOR_REV;
extern const CPU_INT08U _OLEDC_RMP_SEQ_RGB;
extern const CPU_INT08U _OLEDC_RMP_SEQ_BGR;
extern const CPU_INT08U _OLEDC_RMP_SCAN_NOR;
extern const CPU_INT08U _OLEDC_RMP_SCAN_REV;
extern const CPU_INT08U _OLEDC_RMP_SPLIT_DISABLE;
extern const CPU_INT08U _OLEDC_RMP_SPLIT_ENABLE;
extern const CPU_INT08U _OLEDC_COLOR_65K;
extern const CPU_INT08U _OLEDC_COLOR_262K;
extern const CPU_INT08U _OLED_C_IMG_HEAD;

/*
*********************************************************************************************************
*                                             OLED SETTINGS
*********************************************************************************************************
*/
extern const CPU_INT08U  _OLEDC_SCREEN_WIDTH;
extern const CPU_INT08U  _OLEDC_SCREEN_HEIGHT;
extern const CPU_INT16U _OLEDC_SCREEN_SIZE;  
extern const CPU_INT08U  _OLEDC_ROW_OFF;
extern const CPU_INT08U  _OLEDC_COL_OFF;

/*
*********************************************************************************************************
*                                             COMMANDS
*********************************************************************************************************
*/
extern const CPU_INT08U  _OLEDC_SET_COL_ADDRESS;
extern const CPU_INT08U  _OLEDC_SET_ROW_ADDRESS;
extern const CPU_INT08U  _OLEDC_WRITE_RAM;
extern const CPU_INT08U  _OLEDC_READ_RAM;
extern const CPU_INT08U  _OLEDC_SET_REMAP;
extern const CPU_INT08U  _OLEDC_SET_START_LINE;
extern const CPU_INT08U  _OLEDC_SET_OFFSET;
extern const CPU_INT08U  _OLEDC_MODE_OFF;
extern const CPU_INT08U  _OLEDC_MODE_ON;
extern const CPU_INT08U  _OLEDC_MODE_NORMAL;
extern const CPU_INT08U  _OLEDC_MODE_INVERSE;
extern const CPU_INT08U  _OLEDC_FUNCTION;
extern const CPU_INT08U  _OLEDC_SLEEP_ON;
extern const CPU_INT08U  _OLEDC_SLEEP_OFF;
extern const CPU_INT08U  _OLED_C_NOP;
extern const CPU_INT08U  _OLED_C_SET_RESET_PRECH;
extern const CPU_INT08U  _OLED_C_ENHANCEMENT;
extern const CPU_INT08U  _OLEDC_CLOCK_DIV;
extern const CPU_INT08U  _OLEDC_VSL;
extern const CPU_INT08U  _OLEDC_GPIO;
extern const CPU_INT08U  _OLEDC_SETSEC_PRECH;
extern const CPU_INT08U  _OLEDC_GREY_SCALE;
extern const CPU_INT08U  _OLEDC_LUT;
extern const CPU_INT08U  _OLEDC_PRECH_VOL;
extern const CPU_INT08U  _OLEDC_VCOMH;
extern const CPU_INT08U  _OLEDC_CONTRAST;
extern const CPU_INT08U  _OLEDC_MASTER_CONTRAST;
extern const CPU_INT08U  _OLEDC_MUX_RATIO;
extern const CPU_INT08U  _OLEDC_COMMAND_LOCK;
extern const CPU_INT08U  _OLEDC_SCROLL_HOR;
extern const CPU_INT08U  _OLEDC_START_MOV;
extern const CPU_INT08U  _OLEDC_STOP_MOV;

/**
 * @brief SPI driver init
 *
 * Sets the drive mode of enable, reset, and dc pins, and starts SPIM1
 */
void oledc_spiDriverInit();

/**
 * @brief OLED C init sequence
 *
 * Sends the various commands to the oled in specific order to start the oled
 */
void oledc_init();

/**
 * @brief OLED C reset
 *
 * Pin setting and resetting sequence for the reset pin to properly reset the oled screen.
 */
void oledc_reset();

/**
 * @brief OLED C enable
 *
 * @param[in] state (1 = on, 0 = off)
 *
 * Set the enable pin high or low, aka on or off
 */
void oledc_enable(CPU_INT08U state);

/**
 * @brief OLED C set font
 *
 * @param[in] font          font type chosen
 * @param[in] color         color of text aka infill in rgb565
 * @param[in] orientation   orientation of the letts
 *
 * Selects a given font from an external text file via bitmapping
 */
void oledc_set_font(const CPU_INT08U *font, CPU_INT16U color, CPU_INT08U orientation);

/**
 * @brief OLED C command
 *
 * @param[in] command         hex command
 * @param[in] args            pointer to command
 * @param[in] args_len        length of command
 *
 * This is how you communicate with the SSD1351. Sends commands via SPI
 */
void oledc_command(CPU_INT08U command, CPU_INT08U *args, CPU_INT16U args_len);

/**
 * @brief OLED C fill screen
 *
 * @param[in] color         color in rgb565
 *
 * Draws a box area over the entire screen in a specified color
 */
void oledc_fill_screen(CPU_INT16U color);

/**
 * @brief OLED C fill screen
 *
 * @param[in] color         color in rgb565
 *
 * Draws a box area over the entire screen in a specified color
 */
void oledc_rectangle(CPU_INT08U col_off, CPU_INT08U row_off, CPU_INT08U col_end, CPU_INT08U row_end, CPU_INT16U color);

/**
 * @brief OLED C image
 *
 * @param[in] img             image bitmap
 * @param[in] column_off      column offset from left of screen
 * @param[in] row_off         row offset from top of screen
 *
 * Draws image form bitmap
 */
void oledc_image(const CPU_INT08U *img, CPU_INT08U column_off, CPU_INT08U row_off);

/**
 * @brief OLED C text
 *
 * @param[in] text         string
 * @param[in] col_off      column offset from left of screen
 * @param[in] row_off      row offset from top of screen
 *
 * Draws a box area over the entire screen in a specified color
 */
void oledc_text( CPU_INT08U *text, CPU_INT16U x, CPU_INT16U y );
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
void oledc_line(CPU_INT08U x1, CPU_INT08U y1, CPU_INT08U x2, CPU_INT08U y2, CPU_INT16U color);

/**
 * @brief OLED C line any
 *
 * @param[in] x1         start x coordinate
 * @param[in] y1         start y coordinate
 * @param[in] x2         end x coordinate
 * @param[in] y2         end y coordinate
 * @param[in] color      string
 *
 * Draws a 1 pixel thick line from x1y1 to x2y2 at any angle
 */
void oledc_line_any(CPU_INT08U x1, CPU_INT08U y1, CPU_INT08U x2, CPU_INT08U y2, CPU_INT16U color);
/**
 * @brief OLED C post init
 *
 * Additional setup, to be called during system init
 */
void OLED_C_PostInit();

/**
 * @brief Asteroids draw border
 *
 * @param[in] color         desired color
 *
 * Draw 1pixel wide border around entire screen in COLOR_GREEN
 */
void asteroids_DrawBorder(CPU_INT16U color);

/**
 * @brief Asteroids game welcome screen
 *
 * First screen you see when "booting up" the game, only shows during first boot
 */
void asteroids_DrawPreGame();

/**
 * @brief Asteroids arena
 *
 * @param[in] state             GameState struct, current game state
 *
 * Draws the main component of the game arena/area without objects
 */
void asteroids_DrawArena(GameState *state);

/**
 * @brief Asteroids game over
 *
 * @param[in] state             GameState struct, current game state
 *
 * If game collision ooccured, this screen gets posted
 * Game over screen for game, and asks for button click to restart game
 */
void asteroids_DrawGameOver(GameState *state);



//gemini code
void ram_pixel_set(CPU_INT08U col, CPU_INT08U row, CPU_INT16U color);
void ram_rectangle_set(CPU_INT08U start_col, CPU_INT08U start_row, CPU_INT08U end_col, CPU_INT08U end_row, CPU_INT16U color);
void oledc_flush_buffer(const CPU_INT08U *buffer, CPU_INT16U size);



#endif

/* [] END OF FILE */