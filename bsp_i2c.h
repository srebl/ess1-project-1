/*
*********************************************************************************************************
*
*                                           I2C BSP LAYER
*
* Filename      : bsp_i2c.h
* Version       : V1.00
* Programmer(s) : Lukas Buchmayer, Gavrilo Stanic, Zobay Rahimi, Sebastian Rath
*********************************************************************************************************
*/

#ifndef  BSP_I2C_MODULE_PRESENT
#define  BSP_I2C_MODULE_PRESENT


/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*
*********************************************************************************************************
*/
#include <I2C_1.h>
#include <cpu.h>

/*
*********************************************************************************************************
*                                                DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/**
 * @brief Initializes the I2C interface.
 *
 * This function sets up the I2C interface for communication with I2C devices.
 * It should be called once during system initialization before any I2C 
 * communication is performed.
 */
void init_i2c();

/**
 * @brief Writes data to an I2C slave device.
 *
 * This function sends data to a specified register of an I2C slave device.
 * The function supports sending a stop condition after the write operation,
 * if specified.
 *
 * @param slave_address The address of the I2C slave device.
 * @param reg The register address to write to.
 * @param buffer The data byte to be written to the register.
 * @param send_stop A boolean flag indicating whether to send a stop condition
 *                  after the write operation (true to send stop, false to not send).
 */
void i2c_master_write(CPU_INT08U slave_address, CPU_INT08U reg, CPU_INT08U buffer, CPU_BOOLEAN send_stop);

/**
 * @brief Reads data from an I2C slave device.
 *
 * This function reads data from a specified register of an I2C slave device
 * and stores it in the provided buffer. The function can read multiple bytes
 * of data in a single operation.
 *
 * @param slave_address The address of the I2C slave device.
 * @param reg The register address to read from.
 * @param buffer Pointer to the buffer where the read data will be stored.
 * @param len The number of bytes to read.
 */
void i2c_master_read(CPU_INT08U slave_address, CPU_INT08U reg, CPU_INT08U *buffer, CPU_INT08U len);

/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/


#endif
