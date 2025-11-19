/*
*********************************************************************************************************
*
*                                       MES1 Embedded Software (RTOS)
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : Lukas Buchmayer
                  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include <cyapicallbacks.h>

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_USER_IF_SIGN_ON                        0u
#define  APP_USER_IF_VER_TICK_RATE                  1u
#define  APP_USER_IF_CPU                            2u
#define  APP_USER_IF_CTXSW                          3u
#define  APP_USER_IF_STATE_MAX                      4u

#define  USE_BSP_TOGGLE                             1u

#define  C6DOFIMU14_DEBUG                           163u
#define  MESSAGE_QUEUE_SIZE                         sizeof(c6dofimu14_axis_t) * 30
#define  MESSAGE_SIZE                               sizeof(c6dofimu14_axis_t)

/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB   App_TaskStartTCB;
static  CPU_STK  App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   App_TaskCOM_TCB;
static  CPU_STK  App_TaskCOMStk[APP_CFG_TASK_COM_STK_SIZE];

static  OS_TCB   App_TaskSENDER_TCB;
static  CPU_STK  App_TaskSENDERStk[APP_CFG_TASK_SENDER_STK_SIZE];

static  OS_TCB   App_TaskRECEIVER_TCB;
static  CPU_STK  App_TaskRECEIVERStk[APP_CFG_TASK_RECEIVER_STK_SIZE];

static OS_TCB  App_Task_GAME_LOOP_TCB;
static CPU_STK App_Task_GAME_LOOP_Stk[APP_CFG_TASK_GAME_LOOP_STK_SIZE];

OS_Q msg_q;
OS_SEM gameState_sem;
static GameState gameState;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart  (void *p_arg);

static  void  App_TaskCOM (void *p_arg);

static  void  App_TaskCreate (void);
static  void  App_ObjCreate  (void);

static void App_TaskSENDER (void *p_arg);
static void App_TaskRECEIVER (void *p_arg);
static void App_Task_GAME_LOOP (void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : Startup Code.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main (void)
{
  OS_ERR  os_err;


  BSP_PreInit();                                              /* Perform BSP pre-initialization.                      */

  CPU_Init();                                                 /* Initialize the uC/CPU services                       */

  OSInit(&os_err);                                            /* Init uC/OS-III.                                      */

  OSTaskCreate((OS_TCB      *)&App_TaskStartTCB,              /* Create the start task                                */
               (CPU_CHAR    *)"Start",
               (OS_TASK_PTR  )App_TaskStart, 
               (void        *)0,
               (OS_PRIO      )APP_CFG_TASK_START_PRIO,
               (CPU_STK     *)&App_TaskStartStk[0],
               (CPU_STK_SIZE )APP_CFG_TASK_START_STK_SIZE_LIMIT,
               (CPU_STK_SIZE )APP_CFG_TASK_START_STK_SIZE,
               (OS_MSG_QTY   )0u,
               (OS_TICK      )0u,
               (void        *)0,
               (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR      *)&os_err);

  OSStart(&os_err);                                            /* Start multitasking (i.e. give control to uC/OS-III).  */
}


/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Note(s)     : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
  OS_ERR       err;
     
 (void)p_arg;

  BSP_PostInit();                                               /* Perform BSP post-initialization functions.       */
  
  BSP_CPU_TickInit();                                           /* Perfrom Tick Initialization                      */



    oledc_spiDriverInit();
    OLED_C_PostInit();
    

    OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    
    
    
    init_i2c();
    c6dofimu14_init();


#if (OS_CFG_STAT_TASK_EN > 0u)
  OSStatTaskCPUUsageInit(&err);
#endif    

#ifdef CPU_CFG_INT_DIS_MEAS_EN
  CPU_IntDisMeasMaxCurReset();
#endif      

  App_TaskCreate();                                             /* Create application tasks.                        */

  App_ObjCreate();                                              /* Create kernel objects                            */

  while (DEF_TRUE) {                                            /* Task body, always written as an infinite loop.   */
    OSTimeDlyHMSM(0, 0, 0, 100, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &err);
  }
}


/*
*********************************************************************************************************
*                                          App_TaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
  /* declare and define function local variables */
  OS_ERR  os_err;

  /* create COM task */
  OSTaskCreate((OS_TCB      *)&App_TaskCOM_TCB,
               (CPU_CHAR    *)"TaskCOM",
               (OS_TASK_PTR  )App_TaskCOM, 
               (void        *)0,
               (OS_PRIO      )APP_CFG_TASK_COM_PRIO,
               (CPU_STK     *)&App_TaskCOMStk[0],
               (CPU_STK_SIZE )APP_CFG_TASK_COM_STK_SIZE_LIMIT,
               (CPU_STK_SIZE )APP_CFG_TASK_COM_STK_SIZE,
               (OS_MSG_QTY   )0u,
               (OS_TICK      )0u,
               (void        *)0,
               (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR      *)&os_err);
/* create SENDER task */
  OSTaskCreate((OS_TCB      *)&App_TaskSENDER_TCB,
               (CPU_CHAR    *)"TaskSENDER",
               (OS_TASK_PTR  )App_TaskSENDER, 
               (void        *)0,
               (OS_PRIO      )APP_CFG_TASK_SENDER_PRIO,
               (CPU_STK     *)&App_TaskSENDERStk[0],
               (CPU_STK_SIZE )APP_CFG_TASK_SENDER_STK_SIZE_LIMIT,
               (CPU_STK_SIZE )APP_CFG_TASK_SENDER_STK_SIZE,
               (OS_MSG_QTY   )0u,
               (OS_TICK      )0u,
               (void        *)0,
               (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR      *)&os_err);
/* create RECEIVER task */
  OSTaskCreate((OS_TCB      *)&App_TaskRECEIVER_TCB,
               (CPU_CHAR    *)"TaskRECEIVER",
               (OS_TASK_PTR  )App_TaskRECEIVER, 
               (void        *)0,
               (OS_PRIO      )APP_CFG_TASK_RECEIVER_PRIO,
               (CPU_STK     *)&App_TaskRECEIVERStk[0],
               (CPU_STK_SIZE )APP_CFG_TASK_RECEIVER_STK_SIZE_LIMIT,
               (CPU_STK_SIZE )APP_CFG_TASK_RECEIVER_STK_SIZE,
               (OS_MSG_QTY   )0u,
               (OS_TICK      )0u,
               (void        *)0,
               (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR      *)&os_err);
  /* create GAME LOOP task */
OSTaskCreate((OS_TCB      *) &App_Task_GAME_LOOP_TCB,
             (CPU_CHAR    *) "Task_GAME_LOOP",
             (OS_TASK_PTR  ) App_Task_GAME_LOOP, 
             (void        *) 0,
             (OS_PRIO      ) APP_CFG_TASK_GAME_LOOP_PRIO,
             (CPU_STK     *) App_Task_GAME_LOOP_Stk,
             (CPU_STK_SIZE ) APP_CFG_TASK_GAME_LOOP_STK_SIZE_LIMIT,
             (CPU_STK_SIZE ) APP_CFG_TASK_GAME_LOOP_STK_SIZE,
             (OS_MSG_QTY   ) 0u,
             (OS_TICK      ) 0u,
             (void        *) 0,
             (OS_OPT       ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
             (OS_ERR      *) &os_err
            );
}



/*
*********************************************************************************************************
*                                          App_ObjCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_ObjCreate (void)
{
  /* declare and define function local variables */
  OS_ERR  os_err;
   OSSemCreate(&gameState_sem,"SEM_GAME_STATE",1,&os_err);
  /* create button semaphore to synchronize button press events */
}


/*
*********************************************************************************************************
*                                          App_TaskCOM()
*
* Description : COM Task checks for available bytes within the UART receive buffer. If correct string is
*               available (e.g. PC -> uC: #abc$ or #Hellor World$), process the message and output a
*               pre-defined string via UART and append the user-defined string via the UART interface.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskCOM()' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Note(s)     : none
*********************************************************************************************************
*/

static  void  App_TaskCOM (void *p_arg)
{
  /* declare and define task local variables */
  OS_ERR       os_err;
  CPU_INT08U   rec_byte = 0x00;
  CPU_INT08U   rx_msg[UART_1_RX_BUFFER_SIZE] = {0};
  CPU_INT08U   idx = 0x00;
  CPU_INT08U   rec_byte_cnt = 0x00;
  CPU_BOOLEAN  str_available = DEF_FALSE;
  
  /* prevent compiler warnings */
  (void)p_arg;
  (void)Start_of_Packet;
  (void)End_of_Packet;
  
  /* start of the endless loop */
  while (DEF_TRUE) {
    rec_byte = uart_get_byte();
    if(rec_byte == Start_of_Packet){
      while(DEF_TRUE){
        rec_byte = uart_get_byte();
        if(rec_byte){
          rx_msg[idx++] = rec_byte;
        }

        OSTimeDlyHMSM(0, 0, 0, 20, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &os_err);

        if(rx_msg[idx-1]==End_of_Packet){
          break;
        }
      }

      rec_byte_cnt = idx-2;
      str_available = DEF_TRUE;
    }else{
      memset(&rx_msg[0],0,sizeof(rx_msg));
      str_available = DEF_FALSE;
      rec_byte = 0x00;
      idx = 0x00;
    }
 
    if(str_available){
      
        
        
        
      memset(&rx_msg[0],0,sizeof(rx_msg));
      str_available = DEF_FALSE;
      rec_byte = 0x00;
      idx = 0x00;
    }
    /* initiate scheduler */
    OSTimeDlyHMSM(0, 0, 0, 100, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &os_err);
  }
}

/*
*********************************************************************************************************
*                                          App_TaskSENDER()
*
* Description : This task reads acceleration data from a 6DOF IMU sensor, logs the data if debugging
*               is enabled, and then delays for a specified time before repeating.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskSENDER()' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Note(s)     : none
*********************************************************************************************************
*/

static  void  App_TaskSENDER (void *p_arg)
{
  /* declare and define task local variables */
  OS_ERR       os_err;
      CPU_TS         ts;
    c6dofimu14_axis_t axis_data = {0, 0, 0};
  
  /* prevent compiler warnings */
    (void)p_arg;
    CPU_INT08S vX =0;
    CPU_INT08S vY = 0;
     CPU_INT08U buffer[20];
       oledc_set_font(guiFont_Tahoma_7_Regular,0xffff,_OLEDC_FO_HORIZONTAL);
  /* start of the endless loop */
  while (DEF_TRUE) {
    // axis data is generally between 0 100, but there is also a position where the sensors get the max
   
    c6dofimu14_read_accel_axis(&axis_data);
    
    if(axis_data.x > 100){
      vX = MAX_PLAYER_SPEED;
    }else{
      vX = (((axis_data.x / 10)-4) * 2);
    }
    if(axis_data.y > 100){
      vY = MAX_PLAYER_SPEED;
    }else{
      vY = (((axis_data.y / 10)-4) * 2);
    }
    OSSemPend(&gameState_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
    gameState.player.vX = vX;
    gameState.player.vY = vY;
    OSSemPost(&gameState_sem,1,&os_err);
    
    /* initiate scheduler */
    OSTimeDlyHMSM(0, 0, 0, 100, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &os_err);
  }
}

/*
*********************************************************************************************************
*                                          App_TaskRECEIVER()
*
* Description : This task reads acceleration data from a 6DOF IMU sensor, logs the data if debugging
*               is enabled, and then delays for a specified time before repeating.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskRECEIVER()' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Note(s)     : none
*********************************************************************************************************
*/

static  void  App_TaskRECEIVER (void *p_arg)
{
  /* declare and define task local variables */
  OS_ERR       os_err;
  
    OS_MSG_SIZE size;
    CPU_TS ts;
    
    c6dofimu14_axis_t *axis_data;
    c6dofimu14_axis_t old_data = {46, 46, 0}; //middle, no z
    
    oledc_fill_screen(0x0000);
    
    CPU_INT08U sizeFactor = 1;
    CPU_INT08U z_axis = 0;
    CPU_INT08U range = 3;
    oledc_set_font(guiFont_Tahoma_7_Regular,0xffff,_OLEDC_FO_HORIZONTAL);
  /* prevent compiler warnings */
    (void)p_arg;
  
  /* start of the endless loop */
  while (DEF_TRUE) {
    axis_data = (c6dofimu14_axis_t*)OSQPend(&msg_q, 20, OS_OPT_PEND_NON_BLOCKING, &size, &ts, &os_err);
    
    //oledc_rectangle(axis_data->x - sizeFactor, axis_data->y - sizeFactor, axis_data->x + sizeFactor, axis_data->y + sizeFactor, 0xffff);
    CPU_INT08U text[] = "score";
    //oledc_text(text, 40,40);
    
    /* initiate scheduler */
    OSTimeDlyHMSM(0, 0, 0, 100, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &os_err);
  }
}

/*
*********************************************************************************************************
*                                          App_Task_GAME_LOOP
*
* Description : This task reads acceleration data from a MessageQueue, inits the OLED C and then prints
                the data as coordinates on the OLED C as a pixel on an X and Y Axis, with an additional
                line for its acceleration on the Z axis placed onto the Y axis.
*
* Argument(s) : p_arg is the argument passed to 'App_Task_RECEIVER' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Note(s)     : none
*********************************************************************************************************
*/
static void App_Task_GAME_LOOP(void *p_arg){
    /* declare and define task local variables */
    OS_ERR os_err;
    OS_MSG_SIZE size;
    CPU_TS ts;

    
    /* prevent compiler warnings */
    (void)p_arg;
    
    /* start of the endless loop */ 
    while (DEF_TRUE) {
        
        /* initiate scheduler */
        OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}

/* END OF FILE */