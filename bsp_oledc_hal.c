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
 * Authors: Lukas Buchmayer, Gavrilo Stanic, Zobay Rahimi, Sebastian Rath
 * 
 * ========================================
*/

#ifndef BSP_OLED_HAL_PRESENT
#define BSP_OLED_HAL_PRESENT
    
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/    
#include <bsp_oledc.h>

/*
*********************************************************************************************************
*                                             DEFINES
*********************************************************************************************************
*/
#define HIGH 1
#define LOW 0

/**
 * @brief HAL GPIO SPIM1 init
 *
 * Set drive modes and start SPIM1
 */
static void hal_SPI_init(){
    OLED_C_Enable_SetDriveMode(OLED_C_Enable_DM_STRONG);
    OLED_C_Reset_SetDriveMode(OLED_C_Reset_DM_STRONG);
    OLED_C_DC_SetDriveMode(OLED_C_DC_DM_STRONG);
    SPIM_1_Start();
}

/**
 * @brief HAL GPIO CS set
 *
 * @param[in] state (1 = on, 0 = off)
 *
 * Set the cs pin high or low, aka on or off
 */
static void hal_gpio_csSet(CPU_INT08U state){
    OLED_C_CS_Write(state);
}

/**
 * @brief HAL GPIO DC set
 *
 * @param[in] state (1 = on, 0 = off)
 *
 * Set the dc pin high or low, aka on or off
 */
static void hal_gpio_dcSet(CPU_INT08U state){
    OLED_C_DC_Write(state);
}

/**
 * @brief HAL GPIO SPI Write
 *
 * @param[in] buf       data to be sent
 * @param[in] bytes     length of data
 *
 * Write via SPI of certain length, "replaces" PutArray
 */
static void hal_spiWrite(CPU_INT08U *buf, CPU_INT16U bytes){
    for(int i = 0; i < bytes; i++){
        SPIM_1_WriteByte(buf[i]);
    }
}

/**
 * @brief HAL GPIO enable set
 *
 * @param[in] state (1 = on, 0 = off)
 *
 * Set the enable pin high or low, aka on or off
 */
static void hal_gpio_enSet(CPU_INT08U state){
    OLED_C_Enable_Write(state);
}

/**
 * @brief HAL GPIO reset set
 *
 * @param[in] state (1 = on, 0 = off)
 *
 * Set the reset pin high or low, aka on or off
 */
static void hal_gpio_rstSet(CPU_INT08U state){
    OLED_C_Reset_Write(state);
}

#endif

/* [] END OF FILE */