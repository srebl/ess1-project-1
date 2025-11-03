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
#ifndef BSP_OLED_PRESENT
#define BSP_OLED_PRESENT
    
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/    
#include <bsp_oledc.h>
#include "bsp_oledc_hal.c"
    

OS_ERR os_err;

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
*                                             REMAMP
*********************************************************************************************************
*/
#define _OLEDC_RMP_INC_HOR         0x00
#define _OLEDC_RMP_INC_VER         0x01
#define _OLEDC_RMP_COLOR_NOR       0x00
#define _OLEDC_RMP_COLOR_REV       0x02
#define _OLEDC_RMP_SEQ_RGB         0x00
#define _OLEDC_RMP_SEQ_BGR         0x04
#define _OLEDC_RMP_SCAN_NOR        0x00
#define _OLEDC_RMP_SCAN_REV        0x10
#define _OLEDC_RMP_SPLIT_DISABLE   0x00
#define _OLEDC_RMP_SPLIT_ENABLE    0x20
#define _OLEDC_COLOR_65K           0x00
#define _OLEDC_COLOR_262K          0x80
#define _OLEDC_IMG_HEAD            0x06

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

/*
*********************************************************************************************************
*                                             COMMANDS
*********************************************************************************************************
*/
const CPU_INT08U  _OLEDC_SET_COL_ADDRESS   = 0x15;
const CPU_INT08U  _OLEDC_SET_ROW_ADDRESS   = 0x75;
const CPU_INT08U  _OLEDC_WRITE_RAM         = 0x5C;
const CPU_INT08U  _OLEDC_READ_RAM          = 0x5D;
const CPU_INT08U  _OLEDC_SET_REMAP         = 0xA0;
const CPU_INT08U  _OLEDC_SET_START_LINE    = 0xA1;
const CPU_INT08U  _OLEDC_SET_OFFSET        = 0xA2;
const CPU_INT08U  _OLEDC_MODE_OFF          = 0xA4;
const CPU_INT08U  _OLEDC_MODE_ON           = 0xA5;
const CPU_INT08U  _OLEDC_MODE_NORMAL       = 0xA6;
const CPU_INT08U  _OLEDC_MODE_INVERSE      = 0xA7;
const CPU_INT08U  _OLEDC_FUNCTION          = 0xAB;
const CPU_INT08U  _OLEDC_SLEEP_ON          = 0xAE;
const CPU_INT08U  _OLEDC_SLEEP_OFF         = 0xAF;
const CPU_INT08U  _OLEDC_NOP              = 0xB0;
const CPU_INT08U  _OLEDC_SET_RESET_PRECH  = 0xB1;
const CPU_INT08U  _OLEDC_ENHANCEMENT      = 0xB2;
const CPU_INT08U  _OLEDC_CLOCK_DIV         = 0xB3;
const CPU_INT08U  _OLEDC_VSL               = 0xB4;
const CPU_INT08U  _OLEDC_GPIO              = 0xB5;
const CPU_INT08U  _OLEDC_SETSEC_PRECH      = 0xB6;
const CPU_INT08U  _OLEDC_GREY_SCALE        = 0xB8;
const CPU_INT08U  _OLEDC_LUT               = 0xB9;
const CPU_INT08U  _OLEDC_PRECH_VOL         = 0xBB;
const CPU_INT08U  _OLEDC_VCOMH             = 0xBE;
const CPU_INT08U  _OLEDC_CONTRAST          = 0xC1;
const CPU_INT08U  _OLEDC_MASTER_CONTRAST   = 0xC7;
const CPU_INT08U  _OLEDC_MUX_RATIO         = 0xCA;
const CPU_INT08U  _OLEDC_COMMAND_LOCK      = 0xFD;
const CPU_INT08U  _OLEDC_SCROLL_HOR        = 0x96;
const CPU_INT08U  _OLEDC_START_MOV         = 0x9E;
const CPU_INT08U  _OLEDC_STOP_MOV          = 0x9F;

static CPU_INT08U _OLEDC_DEFAULT_MUX_RATIO      = 95;
static CPU_INT08U _OLEDC_DEFAULT_START_LINE     = 0x80;
static CPU_INT08U _OLEDC_DEFAULT_OFFSET         = 0x20;

static CPU_INT08U _OLEDC_DEFAULT_OLED_LOCK      = 0x12;
static CPU_INT08U _OLEDC_DEFAULT_CMD_LOCK       = 0xB1;
static CPU_INT08U _OLEDC_DEFAULT_DIVSET         = 0xF1;
static CPU_INT08U _OLEDC_DEFAULT_PRECHARGE      = 0x32;
static CPU_INT08U _OLEDC_DEFAULT_VCOMH          = 0x05;
static CPU_INT08U _OLEDC_DEFAULT_MASTER_CONT    = 0xCF;
static CPU_INT08U _OLEDC_DEFAULT_PRECHARGE_2    = 0x01;

static CPU_INT08U cols[ 2 ]    = { _OLEDC_COL_OFF, _OLEDC_COL_OFF + 95 };          
static CPU_INT08U rows[ 2 ]    = { _OLEDC_ROW_OFF, _OLEDC_ROW_OFF + 95 };

static CPU_INT08U _OLEDC_DEFAULT_REMAP = _OLEDC_RMP_INC_HOR | _OLEDC_RMP_COLOR_REV |
                                _OLEDC_RMP_SEQ_RGB | _OLEDC_RMP_SCAN_REV |
                                _OLEDC_RMP_SPLIT_ENABLE | _OLEDC_COLOR_65K;

static  CPU_INT08U _OLEDC_DEFAULT_VSL[ 3 ]       = { 0xA0, 0xB5, 0x55 };
static  CPU_INT08U _OLEDC_DEFAULT_CONTRAST[ 3 ]  = { 0x8A, 0x51, 0x8A };

static const CPU_INT08U*   _font;
static CPU_INT16U         _font_color;
static CPU_INT16U          _font_orientation;
static CPU_INT16U         _font_first_char;
static CPU_INT16U         _font_last_char;
static CPU_INT16U         _font_height;
//static CPU_INT16U         x_cord;
//static CPU_INT16U         y_cord;


/**
 * @brief SPI driver init
 *
 * Sets the drive mode of enable, reset, and dc pins, and starts SPIM1
 */
void oledc_spiDriverInit(){
    hal_SPI_init();
    hal_gpio_csSet(HIGH);
    hal_gpio_dcSet(HIGH);
    
}

/**
 * @brief OLED C init sequence
 *
 * Sends the various commands to the oled in specific order to start the oled
 */
void oledc_init(){
    oledc_enable(HIGH);
    oledc_reset();
    
    oledc_command( _OLEDC_COMMAND_LOCK,    &_OLEDC_DEFAULT_OLED_LOCK,     1 );
    oledc_command( _OLEDC_COMMAND_LOCK,    &_OLEDC_DEFAULT_CMD_LOCK,      1 );
    oledc_command( _OLEDC_SLEEP_ON,        0,                      0 );
    
    oledc_command( _OLEDC_SET_REMAP,       &_OLEDC_DEFAULT_REMAP,         1 );
    oledc_command( _OLEDC_MUX_RATIO,       &_OLEDC_DEFAULT_MUX_RATIO,     1 );
    oledc_command( _OLEDC_SET_START_LINE,  &_OLEDC_DEFAULT_START_LINE,    1 );
    oledc_command( _OLEDC_SET_OFFSET,      &_OLEDC_DEFAULT_OFFSET,        1 );
    oledc_command( _OLEDC_VCOMH,           &_OLEDC_DEFAULT_VCOMH,         1 );
    oledc_command( _OLEDC_CLOCK_DIV,       &_OLEDC_DEFAULT_DIVSET,        1 );
    oledc_command( _OLEDC_SET_RESET_PRECH, &_OLEDC_DEFAULT_PRECHARGE,     1 );
    oledc_command( _OLEDC_SETSEC_PRECH,    &_OLEDC_DEFAULT_PRECHARGE_2,   1 );
    oledc_command( _OLEDC_MASTER_CONTRAST, &_OLEDC_DEFAULT_MASTER_CONT,   1 );
    oledc_command( _OLEDC_CONTRAST,        _OLEDC_DEFAULT_CONTRAST,       3 );
    oledc_command( _OLEDC_VSL,             _OLEDC_DEFAULT_VSL,            3 );

    oledc_command(_OLEDC_MODE_NORMAL, 0, 0);
    
    oledc_command(_OLEDC_SLEEP_OFF, 0, 0);
}

/**
 * @brief OLED C reset
 *
 * Pin setting and resetting sequence for the reset pin to properly reset the oled screen.
 */
void oledc_reset(){
    hal_gpio_rstSet(HIGH);
    OSTimeDlyHMSM(0, 0, 0, 1, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &os_err);
    hal_gpio_rstSet(LOW);
    OSTimeDlyHMSM(0, 0, 0, 1, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &os_err);
    hal_gpio_rstSet(HIGH);
    OSTimeDlyHMSM(0, 0, 0, 100, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &os_err);
}

/**
 * @brief OLED C enable
 *
 * @param[in] state (1 = on, 0 = off)
 *
 * Set the enable pin high or low, aka on or off
 */
void oledc_enable(CPU_INT08U state){
    hal_gpio_enSet(state);
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
void oledc_set_font(const CPU_INT08U *font, CPU_INT16U color, CPU_INT08U orientation){
    _font               = font;
    _font_first_char    = font[2] + (font[3] << 8);
    _font_last_char     = font[4] + (font[5] << 8);
    _font_height        = font[6];
    _font_color         = color;
    _font_orientation   = orientation ;
}

/**
 * @brief OLED C function
 *
 * @param[in] col           column of pixel
 * @param[in] row           row of pixel
 * @param[in] color         pixel color in rgb565
 *
 * Draws a pixel at a specified coordination with a specfied color
 */
static void pixel(CPU_INT08U col, CPU_INT08U row, CPU_INT16U color){
    CPU_INT08U cmd = _OLEDC_WRITE_RAM;
    CPU_INT08U clr[2] = {0};
    
    if((col > OLEDC_SCREENSIZE) || (row > OLEDC_SCREENSIZE)){
        return;
    }
    
    cols[0] = _OLEDC_COL_OFF + col;
    cols[1] = _OLEDC_COL_OFF + col;
    rows[0] = _OLEDC_ROW_OFF + row;
    rows[1] = _OLEDC_ROW_OFF + row;
    
    clr[0] |= color >> 8;
    clr[1] |= color & 0x00FF;
    
    oledc_command(_OLEDC_SET_COL_ADDRESS, cols, 2);
    oledc_command(_OLEDC_SET_ROW_ADDRESS, rows, 2);
    
    hal_gpio_csSet(LOW);
    hal_gpio_dcSet(LOW);
    hal_spiWrite(&cmd, 1);
    hal_gpio_dcSet(HIGH);
    
    hal_spiWrite(clr, 2);
    hal_gpio_csSet(HIGH);
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
static void box_area(CPU_INT08U start_col, CPU_INT08U start_row, CPU_INT08U end_col, CPU_INT08U end_row, CPU_INT16U color){
    CPU_INT08U cmd = _OLEDC_WRITE_RAM;
    CPU_INT16U cnt = (end_col - start_col) * (end_row - start_row);
    CPU_INT08U clr[2] = {0};
    
    if((start_col > _OLEDC_SCREEN_WIDTH) || (end_col > _OLEDC_SCREEN_WIDTH)){
        return;
    }
    
    if((start_row > _OLEDC_SCREEN_HEIGHT) || (end_row > _OLEDC_SCREEN_HEIGHT)){
        return;
    }
    
    if((start_col > end_col) || (start_row > end_row)){
        return;
    }
    
    cols[0] = _OLEDC_COL_OFF + start_col;
    cols[1] = _OLEDC_COL_OFF + end_col - 1;
    rows[0] = _OLEDC_ROW_OFF + start_row;
    rows[1] = _OLEDC_ROW_OFF + end_row - 1;
    
    clr[0] |= color >> 8;
    clr[1] |= color & 0x00FF;
    
    oledc_command(_OLEDC_SET_COL_ADDRESS, cols, 2);
    oledc_command(_OLEDC_SET_ROW_ADDRESS, rows, 2);
    
    hal_gpio_csSet(LOW);
    hal_gpio_dcSet(LOW);
    hal_spiWrite(&cmd, 1);
    hal_gpio_dcSet(HIGH);
    
    while(cnt--){
        hal_spiWrite(clr, 2);
    }
    
    hal_gpio_csSet(HIGH);
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
static void draw_area( CPU_INT08U start_col, CPU_INT08U start_row, CPU_INT08U end_col, CPU_INT08U end_row, const CPU_INT08U *img )
{
    CPU_INT16U tmp  = 0;
    CPU_INT08U cmd  = _OLEDC_WRITE_RAM;
    CPU_INT08U frb  = 0;
    CPU_INT08U srb  = 0;
    CPU_INT16U cnt  = ( end_col - start_col ) * ( end_row - start_row );
    
    const CPU_INT08U* ptr = img + _OLEDC_IMG_HEAD;

    if( ( start_col > _OLEDC_SCREEN_WIDTH ) ||
        ( end_col > _OLEDC_SCREEN_WIDTH ) )
        return;

    if( ( start_row > _OLEDC_SCREEN_HEIGHT ) ||
        ( end_row > _OLEDC_SCREEN_HEIGHT ) )
        return;

    if( ( end_col < start_col ) ||
        ( end_row < start_row ) )
        return;

    cols[ 0 ] = _OLEDC_COL_OFF + start_col;
    cols[ 1 ] = _OLEDC_COL_OFF + end_col - 1;
    rows[ 0 ] = _OLEDC_ROW_OFF + start_row;
    rows[ 1 ] = _OLEDC_ROW_OFF + end_row - 1;

    oledc_command( _OLEDC_SET_COL_ADDRESS, cols, 2 );
    oledc_command( _OLEDC_SET_ROW_ADDRESS, rows, 2 );
    hal_gpio_csSet(LOW);
    hal_gpio_dcSet(LOW);
    hal_spiWrite( &cmd, 1 );
    hal_gpio_dcSet(HIGH);
    
    while( cnt-- )
    {
        frb = ptr[ tmp + 1 ];
        srb = ptr[ tmp ];
        hal_spiWrite( &frb, 1 );
        hal_spiWrite( &srb, 1 );
        tmp += 2;
    }
    hal_gpio_csSet(HIGH);
}

/**
 * @brief OLED C command
 *
 * @param[in] command         hex command
 * @param[in] args            pointer to command
 * @param[in] args_len        length of command
 *
 * This is how you communicate with the SSD1351. Sends commands via SPI
 */
void oledc_command(CPU_INT08U command, CPU_INT08U *args, CPU_INT16U args_len){
    CPU_INT08U *ptr = args;
    
    hal_gpio_csSet(LOW);
    hal_gpio_dcSet(LOW);
    
    hal_spiWrite(&command, 1);
    
    hal_gpio_dcSet(HIGH);
    
    if(args_len){
        hal_spiWrite(ptr, args_len);
    }

    hal_gpio_csSet(HIGH);
}

/**
 * @brief OLED C fill screen
 *
 * @param[in] color         color in rgb565
 *
 * Draws a box area over the entire screen in a specified color
 */
void oledc_fill_screen(CPU_INT16U color){
    box_area(0, 0, 96, 96, color);
}

/**
 * @brief OLED C fill screen
 *
 * @param[in] color         color in rgb565
 *
 * Draws a box area over the entire screen in a specified color
 */
void oledc_rectangle(CPU_INT08U col_off, CPU_INT08U row_off, CPU_INT08U col_end, CPU_INT08U row_end, CPU_INT16U color){
    box_area(col_off, row_off, col_end, row_end, color);
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
void oledc_image(const CPU_INT08U *img, CPU_INT08U column_off, CPU_INT08U row_off){
    const CPU_INT08U *ptr = img;
    draw_area( column_off, row_off, column_off + ptr[2], row_off + ptr[4], ptr );

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
void oledc_text(CPU_INT08U *text, CPU_INT16U col_off, CPU_INT16U row_off){
    (void)text;
    (void)col_off;
    (void)row_off;
    //to be added
}

/**
 * @brief OLED C post init
 *
 * Additional setup, to be called during system init
 */
void OLED_C_PostInit(){
    oledc_init();                     
    oledc_fill_screen(0xFFFF);         
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
void oledc_line(CPU_INT08U x1, CPU_INT08U y1, CPU_INT08U x2, CPU_INT08U y2, CPU_INT16U color){
    if(x1 == x2){
        if(y1 > y2){
            int temp = y1; y1 = y2; y2 = temp;
        }
        oledc_rectangle(x1, y1, x1 + 1, y2, color);
    } else if(y1 == y2) {
        if(x1 > x2){
            int temp = x1; x1 = x2; x2 = temp;
        }
        oledc_rectangle(x1, y1, x2, y1 + 1, color);
    }
}

/**
 * @brief OLED C hud
 *
 * Draws the main hud "background"
 */
void oledc_hud(){
    oledc_line(48, 0, 48, 95,0x07e0);
    oledc_line(0, 48, 95, 48,0x07e0);
}


#endif

/* [] END OF FILE */