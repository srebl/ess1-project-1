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
#include "bsp_oledc_dma.h"

/* DMA Configuration for DMA_oled */ 
#define DMA_oled_BYTES_PER_BURST        (1u) 
#define DMA_oled_REQUEST_PER_BURST      (1u) 
#define DMA_oled_SRC_BASE               (CYDEV_SRAM_BASE) 
#define DMA_oled_DST_BASE               (CYDEV_PERIPH_BASE) 
#define FRAME_BYTES                     (96 * 96 * 2)

/* Variable declarations for DMA_oled */
static const CPU_INT16U td0_size = 4095u;
static const CPU_INT16U td1_size = 4095u;
static const CPU_INT16U td2_size = 4095u;
static const CPU_INT16U td3_size = 4095u;
static const CPU_INT16U td4_size = 2052u;
    
static CPU_INT08U buffer_a[FRAME_BYTES];
static CPU_INT08U buffer_b[FRAME_BYTES];
static CPU_INT08U *buffer_front_ptr = buffer_a;
static CPU_INT08U *buffer_back_ptr = buffer_b;
static CPU_INT08U DMA_oled_Chan;
static CPU_INT08U DMA_oled_TD[5];

static OS_SEM *sem_oled_dma_done_ptr;

/* static function prototypes */
static void OLED_DMA_set_buffer(CPU_INT08U *buffer);

CY_ISR(isr_OLED_DMA_Interrupt_callback){
    isr_OLED_DMA_ClearPending();
    OS_ERR os_err;
    OSSemPost(sem_oled_dma_done_ptr, OS_OPT_POST_1, &os_err);
}

void OLED_DMA_Init(OS_SEM *sem_ptr)
{
    sem_oled_dma_done_ptr = sem_ptr;
    // allocate and init channel
    DMA_oled_Chan = DMA_oled_DmaInitialize
    (
        DMA_oled_BYTES_PER_BURST, 
        DMA_oled_REQUEST_PER_BURST, 
        HI16(DMA_oled_SRC_BASE), 
        HI16(DMA_oled_DST_BASE)
    );
    
    // allocate TD
    DMA_oled_TD[0] = CyDmaTdAllocate();
    DMA_oled_TD[1] = CyDmaTdAllocate();
    DMA_oled_TD[2] = CyDmaTdAllocate();
    DMA_oled_TD[3] = CyDmaTdAllocate();
    DMA_oled_TD[4] = CyDmaTdAllocate();
    
    /* set TD configurations */
    CyDmaTdSetConfiguration
    (
        DMA_oled_TD[0], 
        td0_size, 
        DMA_oled_TD[1], 
        CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_AUTO_EXEC_NEXT
    );
    CyDmaTdSetConfiguration
    (
        DMA_oled_TD[1], 
        td1_size, 
        DMA_oled_TD[2], 
        CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_AUTO_EXEC_NEXT
    );
    CyDmaTdSetConfiguration
    (
        DMA_oled_TD[2], 
        td2_size, 
        DMA_oled_TD[3], 
        CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_AUTO_EXEC_NEXT
    );
    CyDmaTdSetConfiguration
    (
        DMA_oled_TD[3], 
        td3_size, 
        DMA_oled_TD[4], 
        CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_AUTO_EXEC_NEXT
    );
    CyDmaTdSetConfiguration
    (
        DMA_oled_TD[4], 
        td4_size, 
        CY_DMA_DISABLE_TD, 
        DMA_oled__TD_TERMOUT_EN | CY_DMA_TD_INC_SRC_ADR
    );
    
    OLED_DMA_set_buffer(buffer_a);
    
    isr_OLED_DMA_StartEx(isr_OLED_DMA_Interrupt_callback);
}

CPU_INT08U * OLED_DMA_get_buffer(){
    return buffer_back_ptr;
}

void OLED_DMA_swap_buffer(){
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    CPU_INT08U *tmp = buffer_front_ptr;
    buffer_front_ptr = buffer_back_ptr;
    buffer_back_ptr = tmp;
    CPU_CRITICAL_EXIT();
}

void OLED_DMA_start_tx(void)
{
    OLED_DMA_set_buffer(buffer_front_ptr);
    CyDmaChEnable(DMA_oled_Chan, 1u);
}

static void OLED_DMA_set_buffer(CPU_INT08U *buffer_ptr)
{
    /* set TD addresses */
    CyDmaTdSetAddress
    (
        DMA_oled_TD[0], 
        LO16((uint32)buffer_ptr), 
        LO16((uint32)SPIM_1_TXDATA_PTR)
    );
    CyDmaTdSetAddress
    (
        DMA_oled_TD[1], 
        LO16((uint32)(buffer_ptr = buffer_ptr + td0_size)), 
        LO16((uint32)SPIM_1_TXDATA_PTR)
    );
    CyDmaTdSetAddress
    (
        DMA_oled_TD[2], 
        LO16((uint32)(buffer_ptr = buffer_ptr + td1_size)), 
        LO16((uint32)SPIM_1_TXDATA_PTR)
    );
    CyDmaTdSetAddress
    (
        DMA_oled_TD[3], 
        LO16((uint32)(buffer_ptr = buffer_ptr + td2_size)), 
        LO16((uint32)SPIM_1_TXDATA_PTR)
    );
    CyDmaTdSetAddress
    (
        DMA_oled_TD[4], 
        LO16((uint32)(buffer_ptr = buffer_ptr + td3_size)), 
        LO16((uint32)SPIM_1_TXDATA_PTR)
    );
    
    /* Attach first TD to channel */
    CyDmaChSetInitialTd(DMA_oled_Chan, DMA_oled_TD[0]);
}
/* [] END OF FILE */
