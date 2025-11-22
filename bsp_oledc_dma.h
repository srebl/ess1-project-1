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

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "stdint.h"
#include "os.h"
#include "SPIM_1.h"
#include "DMA_oled_dma.h"
#include "isr_OLED_DMA.h"

void OLED_DMA_Init(OS_SEM *sem_ptr);
CPU_INT08U * OLED_DMA_get_buffer();
void OLED_DMA_swap_buffer();
void OLED_DMA_start_tx(void);
/* [] END OF FILE */
