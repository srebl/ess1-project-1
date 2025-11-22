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
#include "raster_utils.h"

static OS_ERR os_err;

/*
*********************************************************************************************************
*                                             FONT ORIENTATIONS
*********************************************************************************************************
*/
const CPU_INT08U  _OLEDC_FO_HORIZONTAL       = 0x00;
const CPU_INT08U  _OLEDC_FO_VERTICAL         = 0x01;
const CPU_INT08U  _OLEDC_FO_VERTICAL_COLUMN  = 0x02;

/*
*********************************************************************************************************
*                                             OLED SETTINGS
*********************************************************************************************************
*/
const CPU_INT08U  _OLEDC_SCREEN_WIDTH    = 0x60;
const CPU_INT08U  _OLEDC_SCREEN_HEIGHT   = 0x60;
const CPU_INT16U _OLEDC_SCREEN_SIZE     = 0x2400;
#define  _OLEDC_ROW_OFF          0x00
#define  _OLEDC_COL_OFF          0x10
#define _OLEDC_IMG_HEAD            0x06
#define COLOR_GREEN     0x3fe7
#define COLOR_WHITE     0x0000
#define COLOR_RED       0xf8c3


static const CPU_INT08U*   _font;
static CPU_INT16U         _font_color;
static CPU_INT16U          _font_orientation;
static CPU_INT16U         _font_first_char;
static CPU_INT16U         _font_last_char;
static CPU_INT16U         _font_height;
static CPU_INT16U         x_cord;
static CPU_INT16U         y_cord;

/**
 * @brief OLED C function
 *
 * @param[in] col           column of pixel
 * @param[in] row           row of pixel
 * @param[in] color         pixel color in rgb565
 *
 * Draws a pixel at a specified coordination with a specfied color
 */
void raster_pixel(CPU_INT08U col, CPU_INT08U row, CPU_INT16U color)
{
    if((col > _OLEDC_SCREEN_WIDTH) || (row > _OLEDC_SCREEN_HEIGHT)){
        return;
    }
   
    CPU_INT08U *buffer = OLED_DMA_get_buffer();
    *(buffer + col + (row * _OLEDC_SCREEN_WIDTH)) = color >> 8;
    *(buffer + col + (row * _OLEDC_SCREEN_WIDTH) + 1) = color & 0x00FF;
}


static void raster_character( uint16_t ch )
{
    uint8_t     ch_width = 0;
    uint8_t     x_cnt;
    uint8_t     y_cnt;
    uint16_t    x = 0;
    uint16_t    y = 0;
    uint16_t    tmp;
    uint8_t     temp = 0;
    uint8_t     mask = 0;
    uint32_t    offset;
    const uint8_t *ch_table;
    const uint8_t *ch_bitmap;

    if( ch < _font_first_char )
        return;

    if( ch > _font_last_char )
        return;

    offset = 0;
    tmp = (ch - _font_first_char) << 2;
    ch_table = _font + 8 + tmp;
    ch_width = *ch_table;

    offset = (uint32_t)ch_table[1] + ((uint32_t)ch_table[2] << 8) + ((uint32_t)ch_table[3] << 16);

    ch_bitmap = _font + offset;

    if( ( _font_orientation == _OLEDC_FO_HORIZONTAL ) ||
        ( _font_orientation == _OLEDC_FO_VERTICAL_COLUMN ) )
    {
        y = y_cord;
        for (y_cnt = 0; y_cnt < _font_height; y_cnt++)
        {
            x = x_cord;
            mask = 0;
            for( x_cnt = 0; x_cnt < ch_width; x_cnt++ )
            {
                if( !mask )
                {
                    temp = *ch_bitmap++;
                    mask = 0x01;
                }

                if( temp & mask )
                    raster_pixel( x, y, _font_color );

                x++;
                mask <<= 1;
            }
            y++;
        }

        if ( _font_orientation == _OLEDC_FO_HORIZONTAL )
            x_cord = x + 1;
        else
            y_cord = y;
    }
    else
    {
        y = x_cord;

        for( y_cnt = 0; y_cnt < _font_height; y_cnt++ )
        {
            x = y_cord;
            mask = 0;

            for( x_cnt = 0; x_cnt < ch_width; x_cnt++ )
            {
                if( mask == 0 )
                {
                    temp = *ch_bitmap++;
                    mask = 0x01;
                }

                if( temp & mask )
                {
                    raster_pixel( y, x, _font_color );
                }

                x--;
                mask <<= 1;
            }
            y++;
        }
        y_cord = x - 1;
    }
}

/**
 * @brief OLED C set font
 *
 * @param[in] font          font type chosen
 * @param[in] color         color of text aka infill in rgb565
 * @param[in] orientation   orientation of the letts
 *
 * Selects a given font from an external text file via bitmapping
 */
void raster_set_font(const CPU_INT08U *font, CPU_INT16U color, CPU_INT08U orientation){
    _font               = font;
    _font_first_char    = font[2] + (font[3] << 8);
    _font_last_char     = font[4] + (font[5] << 8);
    _font_height        = font[6];
    _font_color         = color;
    _font_orientation   = orientation ;
}

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
static void raster_box_area(CPU_INT08U start_col, CPU_INT08U start_row, CPU_INT08U end_col, CPU_INT08U end_row, CPU_INT16U color)
{
    if((start_col > _OLEDC_SCREEN_WIDTH) || (end_col > _OLEDC_SCREEN_WIDTH)){
        return;
    }
    
    if((start_row > _OLEDC_SCREEN_HEIGHT) || (end_row > _OLEDC_SCREEN_HEIGHT)){
        return;
    }
    
    if((start_col > end_col) || (start_row > end_row)){
        return;
    }
    
    for(CPU_INT08U row = start_row; start_row <= end_row; row++)
    {
        for(CPU_INT08U col = start_col; start_col <= end_col; col++)
        {
            raster_pixel(col, row, color);
        }         
    }
}

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
void raster_draw_area( CPU_INT08U start_col, CPU_INT08U start_row, CPU_INT08U end_col, CPU_INT08U end_row, const CPU_INT08U *img )
{
    if
    ( 
        ( start_col > _OLEDC_SCREEN_WIDTH ) ||
        ( end_col > _OLEDC_SCREEN_WIDTH ) 
    )
    {
        return;
    }

    if
    ( 
        ( start_row > _OLEDC_SCREEN_HEIGHT ) ||
        ( end_row > _OLEDC_SCREEN_HEIGHT ) 
    )
    {
        return;
    }
    
    if
    ( 
        ( end_col < start_col ) ||
        ( end_row < start_row ) 
    )
    {
        return;
    }
    
    CPU_INT16U color = 0;
    CPU_INT16U tmp  = 0;    
    const CPU_INT08U* ptr = img + _OLEDC_IMG_HEAD;

    for(CPU_INT08U row = start_row; start_row <= end_row; row++)
    {
        for(CPU_INT08U col = start_col; start_col <= end_col; col++)
        {
            color = ((ptr[ tmp + 1 ]) << 8) + (ptr[tmp]);
            tmp += 2;
            raster_pixel(col, row, color);
        }         
    }
}

/**
 * @brief OLED C fill screen
 *
 * @param[in] color         color in rgb565
 *
 * Draws a box area over the entire screen in a specified color
 */
void raster_fill_screen(CPU_INT16U color){
    raster_box_area(0, 0, 96, 96, color);
}

/**
 * @brief OLED C fill screen
 *
 * @param[in] color         color in rgb565
 *
 * Draws a box area over the entire screen in a specified color
 */
void raster_rectangle(CPU_INT08U col_off, CPU_INT08U row_off, CPU_INT08U col_end, CPU_INT08U row_end, CPU_INT16U color){
    raster_box_area(col_off, row_off, col_end, row_end, color);
}

/**
 * @brief OLED C image
 *
 * @param[in] img             image bitmap
 * @param[in] column_off      column offset from left of screen
 * @param[in] row_off         row offset from top of screen
 *
 * Draws image form bitmap
 */
void raster_image(const CPU_INT08U *img, CPU_INT08U column_off, CPU_INT08U row_off){
    const CPU_INT08U *ptr = img;
    raster_draw_area( column_off, row_off, column_off + ptr[2], row_off + ptr[4], ptr );

}

/**
 * @brief OLED C text
 *
 * @param[in] text         string
 * @param[in] col_off      column offset from left of screen
 * @param[in] row_off      row offset from top of screen
 *
 * Draws a box area over the entire screen in a specified color
 */
void raster_text( CPU_INT08U *text, CPU_INT16U x, CPU_INT16U y )
{
    CPU_INT08U *ptr = text;

    if( ( x >= _OLEDC_SCREEN_WIDTH ) || (y >= _OLEDC_SCREEN_HEIGHT ) )
        return;

    x_cord = x;
    y_cord = y;

    while( *ptr )
        raster_character( *ptr++ );
}

/**
 * @brief OLED C post init
 *
 * Additional setup, to be called during system init
 */
void raster_OLED_C_PostInit(){                  
    raster_fill_screen(0xFFFF);         
}

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
void raster__line(CPU_INT08U x1, CPU_INT08U y1, CPU_INT08U x2, CPU_INT08U y2, CPU_INT16U color){
    if(x1 == x2){
        if(y1 > y2){
            int temp = y1; y1 = y2; y2 = temp;
        }
        raster_rectangle(x1, y1, x1 + 1, y2, color);
    } else if(y1 == y2) {
        if(x1 > x2){
            int temp = x1; x1 = x2; x2 = temp;
        }
        raster_rectangle(x1, y1, x2, y1 + 1, color);
    }
}

void raster__line_any(CPU_INT08U x1, CPU_INT08U y1, CPU_INT08U x2, CPU_INT08U y2, CPU_INT16U color){
    if(x1 != x2 && y1 != y2){
        CPU_INT08S d_x = x2 - x1;
        CPU_INT08S d_y = y2 - y1;
        
        CPU_INT16S steps = (abs(d_x) > abs(d_y)) ? abs(d_x) : abs(d_y);
        
        CPU_FP32 x_inc = (CPU_FP32)d_x / steps;
        CPU_FP32 y_inc = (CPU_FP32)d_y / steps;
        
        CPU_FP32 x = x1;
        CPU_FP32 y = y1;
        
        for(CPU_INT08U i = 0; i <= steps; i++){
            raster_pixel((CPU_INT08U)round(x), (CPU_INT08U)round(y), color);
            x += x_inc;
            y += y_inc;
        }
    }
}

/**
 * @brief Asteroids draw border
 *
 * Draw 1pixel wide border around entire screen in COLOR_GREEN
 */
void raster_asteroids_DrawBorder(){
    raster_line(0, 0, 0, 95, COLOR_GREEN);
    raster_line(0, 0, 95, 0, COLOR_GREEN);
    raster_line(0, 95, 95, 95, COLOR_GREEN);
    raster_line(95, 0, 95, 95, COLOR_GREEN);
}

/**
 * @brief Asteroids game welcome screen
 *
 * First screen you see when "booting up" the game, only shows during first boot
 */
void raster_asteroids_DrawPreGame(){
    raster_set_font(guiFont_Tahoma_7_Regular,COLOR_GREEN,_OLEDC_FO_HORIZONTAL);
    raster_asteroids_DrawBorder();
    
    raster_line(20, 15, 75, 15, COLOR_GREEN);
    CPU_INT08U title[] = "ASTEROIDS";
    raster_text(title, 20, 25);
    
    raster_line(10, 45, 85, 45, COLOR_GREEN);
    oledc_line(15, 55, 80, 55, COLOR_GREEN);
    
    raster_line_any(20, 15, 10, 45, COLOR_GREEN);
    raster_line_any(75, 15, 85, 45, COLOR_GREEN);
    
    raster_line_any(10, 45, 15, 55, COLOR_GREEN);
    raster_line_any(80, 55, 85, 45, COLOR_GREEN);
    
    
    CPU_INT08U subtitle[] = "Press BTN to Start";
    raster_text(subtitle, 8, 65);
}

/**
 * @brief Asteroids arena
 *
 * Draws the main component of the game arena/area without objects
 */
void raster_asteroids_DrawArena(){
    raster_set_font(guiFont_Tahoma_7_Regular,COLOR_GREEN,_OLEDC_FO_HORIZONTAL);
    raster_asteroids_DrawBorder();
}

/**
 * @brief Asteroids game over
 *
 * If game collision ooccured, this screen gets posted
 * Game over screen for game, and asks for button click to restart game
 */
void raster_asteroids_DrawGameOver(){
    raster_set_font(guiFont_Tahoma_7_Regular,COLOR_RED,_OLEDC_FO_HORIZONTAL);
    raster_asteroids_DrawBorder();
    
    CPU_INT08U title1[] = "GAME";
    raster_text(title1, 20, 25);
    CPU_INT08U title2[] = "OVER";
    raster_text(title2, 30, 25);
}


/* [] END OF FILE */
