/*
*********************************************************************************************************
*
*                                        C6DOFIMU14 SUPPORT PACKAGE
*
* Filename      : bsp_c6dofimu14.c
* Version       : V1.00
* Programmer(s) : Lukas Buchmayer
*********************************************************************************************************
*/

#include <bsp_c6dofimu14.h>


/**
 * @brief Maps a given integer from one range to another.
 *
 * This function linearly maps a given integer x from the range 
 * in_min and in_max to the range out_min and out_max.

 *
 * @param x The input value to be mapped.
 * @param in_min The lower bound of the input range.
 * @param in_max The upper bound of the input range.
 * @param out_min The lower bound of the output range.
 * @param out_max The upper bound of the output range.
 * 
 * @return The mapped value in the range out_min and out_max.
 */
static CPU_INT16S map(CPU_INT16S x, CPU_INT16S in_min, CPU_INT16S in_max, CPU_INT16S out_min, CPU_INT16S out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

/**
 * @brief Checks if new data is ready from the 6-DOF IMU sensor.
 *
 * This function checks the C6DOFIMU14_REG0_INT_STATUS_1 register of the
 * 6-DOF IMU sensor to determine if new data is ready. It reads the status
 * using I2C communication and delays between each check. If the data is
 * ready within a specified timeout period, the function returns
 * a success code. If the timeout period is exceeded the function returns 
 * an error code.
 *
 * @return C6DOFIMU14_OK if new data is ready, otherwise C6DOFIMU14_ERR if a timeout occurs.
 */
static CPU_INT08U c6dofimu14_check_data_ready() {
    OS_ERR os_err;
    uint8_t tmp_data;
    uint16_t timeout_cnt = 0;
    for ( ; ; )
    {
        i2c_master_read(C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_INT_STATUS_1, &tmp_data, 1);
        OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &os_err);
        
        if ( tmp_data & 0x08 )
        {
            return C6DOFIMU14_OK;
        }
        if ( timeout_cnt++ > 1000 )
        {
            return C6DOFIMU14_ERR;
        }
    }
}

void c6dofimu14_init(){
    OS_ERR os_err;
    CPU_INT08U tmp_data = C6DOFIMU14_DEVCONFIG_SOFT_RESET_ENABLE;
    i2c_master_write (C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_DEVICE_CONFIG, tmp_data, DEF_TRUE);
    
    OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &os_err);

    tmp_data = C6DOFIMU14_DRIVECONFIG_MIN_SLEW_RATE;
    i2c_master_write ( C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_DRIVE_CONFIG, tmp_data, DEF_TRUE);
    
    
    while (tmp_data != C6DOFIMU14_DEF_WHO_AM_I){
        i2c_master_read ( C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_WHO_AM_I, &tmp_data, 1 );
    };
    
    tmp_data = C6DOFIMU14_PWR_TEMP_DISABLE | 
               C6DOFIMU14_PWR_IDLE_1 |
               C6DOFIMU14_PWR_GYRO_MODE_OFF |
               C6DOFIMU14_PWR_ACCEL_MODE_LOW_NOISE;
    
    i2c_master_write(C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_PWR_MGMT_0, tmp_data, DEF_TRUE);
    tmp_data = C6DOFIMU14_ACFG0_FS_SEL_16G |
               C6DOFIMU14_ACFG0_ODR_1KHZ;
    i2c_master_write (C6DOFIMU14_ADDRESS,  C6DOFIMU14_REG0_ACCEL_CONFIG_0, tmp_data, DEF_TRUE);
    
    tmp_data = C6DOFIMU14_GCFG0_FS_SEL_2000DPS |
               C6DOFIMU14_GCFG0_ODR_1KHZ;
    i2c_master_write (C6DOFIMU14_ADDRESS,  C6DOFIMU14_REG0_GYRO_CONFIG_0, tmp_data, DEF_TRUE);
    
    tmp_data = C6DOFIMU14_FIFOCONFIG_STREAM_TO_FIFO_MODE;
    i2c_master_write (C6DOFIMU14_ADDRESS,  C6DOFIMU14_REG0_FIFO_CONFIG, tmp_data, DEF_TRUE);
    
    tmp_data = C6DOFIMU14_FIFOCONFIG1_TEMP_EN |
               C6DOFIMU14_FIFOCONFIG1_GYRO_EN |
               C6DOFIMU14_FIFOCONFIG1_ACCEL_EN;
    i2c_master_write (C6DOFIMU14_ADDRESS,  C6DOFIMU14_REG0_FIFO_CONFIG_1, tmp_data, DEF_TRUE);
    
    OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &os_err);
    
}
    
    
void c6dofimu14_read_accel_axis(c6dofimu14_axis_t *axis){
    CPU_INT08U tmp_data;
    
    while (c6dofimu14_check_data_ready() != C6DOFIMU14_OK){};
    
    
    i2c_master_read(C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_ACCEL_X_MSB, &tmp_data, 1 );
    axis->x = tmp_data;
    axis->x <<= 8;
    i2c_master_read( C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_ACCEL_X_LSB, &tmp_data, 1 );
    axis->x |= tmp_data;
    axis->x = map(axis->x, C6DOFIMU14_ACCEL_MIN, C6DOFIMU14_ACCEL_MAX, OLED_X_MIN, OLED_X_MAX);

    i2c_master_read( C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_ACCEL_Y_MSB, &tmp_data, 1 );
    axis->y = tmp_data;
    axis->y <<= 8;
    i2c_master_read( C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_ACCEL_Y_LSB, &tmp_data, 1 );
    axis->y = map(axis->y, C6DOFIMU14_ACCEL_MIN, C6DOFIMU14_ACCEL_MAX, OLED_Y_MIN, OLED_Y_MAX);

    i2c_master_read( C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_ACCEL_Z_MSB, &tmp_data, 1 );
    axis->z = tmp_data;
    axis->z <<= 8;
    i2c_master_read( C6DOFIMU14_ADDRESS, C6DOFIMU14_REG0_ACCEL_Z_LSB, &tmp_data, 1 );
    axis->z |= tmp_data;
    axis->z = map(axis->z, C6DOFIMU14_ACCEL_MIN, C6DOFIMU14_ACCEL_MAX, OLED_X_MIN, OLED_X_MAX);

}
    
/* [] END OF FILE */
