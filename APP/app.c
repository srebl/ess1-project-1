/*
*********************************************************************************************************
*
*                                       MES1 Embedded Software (RTOS)
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : Lukas Buchmayer, Gavrilo Stanic, Zobay Rahimi, Sebastian Rath
                  
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

#define INPUT_TASK_DELAY                            10
#define GAME_LOOP_TASK_DELAY                        50
#define RENDERER_TASK_DELAY                         50
#define DISPLAY_TASK_DELAY                          50

#define CENTER_POINT                                45.0f
#define DEAD_ZONE_WIDTH                             10.0f // +/- 5.0f around 50.0f
#define MAX_ACCEL_VALUE                             100.0f
#define MAX_PLAYER_SPEED_F                          6.0f  // Float maximum speed

#define FORWARD_THRESHOLD_Y                         40 //game restart


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

static OS_TCB  App_Task_RENDERER_TCB;
static CPU_STK App_Task_RENDERER_Stk[APP_CFG_TASK_RENDERER_STK_SIZE];

OS_Q msg_q;
OS_SEM gameState_sem;
OS_SEM frame_buffer_sem;
OS_SEM gyro_sem;

static GameState gameState;

typedef struct{
  Rectangle rectangles[MAX_ASTEROIDS];
  CPU_INT08U player_x,player_y;
}FrameBuffer;

static FrameBuffer frame_buffer_copy;
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
static void App_Task_RENDERER (void *p_arg);

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
    
    OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    
    oledc_fill_screen(COLOR_BLACK);
    asteroids_DrawPreGame();
    
    OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    
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
OSTaskCreate((OS_TCB      *) &App_Task_RENDERER_TCB,
             (CPU_CHAR    *) "Task_RENDERER",
             (OS_TASK_PTR  ) App_Task_RENDERER, 
             (void        *) 0,
             (OS_PRIO      ) APP_CFG_TASK_RENDERER_PRIO,
             (CPU_STK     *) App_Task_RENDERER_Stk,
             (CPU_STK_SIZE ) APP_CFG_TASK_RENDERER_STK_SIZE_LIMIT,
             (CPU_STK_SIZE ) APP_CFG_TASK_RENDERER_STK_SIZE,
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
  
    //create semaphores for frame buffer and game state
    OSSemCreate(&gameState_sem,"SEM_GAME_STATE",1,&os_err);
    OSSemCreate(&frame_buffer_sem,"SEM_FRAME_BUFFER",1,&os_err);
    OSSemCreate(&gyro_sem, "SEM_GYRO", 0, &os_err);

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
* Description : This task reads acceleration data from a 6DOF IMU sensor and assigns the read data to 
                the GameState struct. Specifically here, only the player struct of the GameState struct.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskSENDER()' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Note(s)     : none
*********************************************************************************************************
*/

static void App_TaskSENDER (void *p_arg)
{
    OS_ERR       os_err;
    CPU_TS       ts;
    c6dofimu14_axis_t axis_data = {0, 0, 0};
  
    (void)p_arg;

    CPU_FP32 vX = 0.0f;
    CPU_FP32 vY = 0.0f;
    
    gameState.mode = GAME_MODE_RUNNING;
    
    while (DEF_TRUE) {
        c6dofimu14_read_accel_axis(&axis_data);
        
        
        if(gameState.mode == GAME_MODE_RUNNING){
            OSSemPend(&gameState_sem, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
            CPU_FP32 inputX = (CPU_FP32)axis_data.x;
            
            if ((inputX >= (CENTER_POINT - DEAD_ZONE_WIDTH/2.0f)) && 
                (inputX <= (CENTER_POINT + DEAD_ZONE_WIDTH/2.0f))) {

                vX = 0.0f;
            } else if (inputX < CENTER_POINT) { 
                //Calculate ratio based on distance from the dead zone edge
                vX = ( (inputX - (CENTER_POINT - DEAD_ZONE_WIDTH/2.0f)) / (CENTER_POINT - DEAD_ZONE_WIDTH/2.0f) ) * MAX_PLAYER_SPEED_F;
                
                //Clamp (no need to cast since vX is already float)
                if (vX < -MAX_PLAYER_SPEED_F) {
                    vX = -MAX_PLAYER_SPEED_F;
                }

            } else { //inputX > CENTER_POINT
                //Calculate ratio based on distance from the dead zone edge
                vX = ( (inputX - (CENTER_POINT + DEAD_ZONE_WIDTH/2.0f)) / (MAX_ACCEL_VALUE - (CENTER_POINT + DEAD_ZONE_WIDTH/2.0f)) ) * MAX_PLAYER_SPEED_F;

                //Clamp
                if (vX > MAX_PLAYER_SPEED_F) {
                    vX = MAX_PLAYER_SPEED_F;
                }
            }

            //Y-AXIS (Vertical)
            CPU_FP32 inputY = (CPU_FP32)axis_data.y;
            
            if ((inputY >= (CENTER_POINT - DEAD_ZONE_WIDTH/2.0f)) && 
                (inputY <= (CENTER_POINT + DEAD_ZONE_WIDTH/2.0f))) {
                vY = 0.0f;
            } else if (inputY < CENTER_POINT) {
                vY = ( (inputY - (CENTER_POINT - DEAD_ZONE_WIDTH/2.0f)) / (CENTER_POINT - DEAD_ZONE_WIDTH/2.0f) ) * MAX_PLAYER_SPEED_F;
                if (vY < -MAX_PLAYER_SPEED_F) { vY = -MAX_PLAYER_SPEED_F; }
            } else { 
                vY = ( (inputY - (CENTER_POINT + DEAD_ZONE_WIDTH/2.0f)) / (MAX_ACCEL_VALUE - (CENTER_POINT + DEAD_ZONE_WIDTH/2.0f)) ) * MAX_PLAYER_SPEED_F;
                if (vY > MAX_PLAYER_SPEED_F) { vY = MAX_PLAYER_SPEED_F; }
            }
            
            gameState.player.vX = vX; //Assigning Float to Float (Safe)
            gameState.player.vY = vY; //Assigning Float to Float (Safe)
            OSSemPost(&gameState_sem,OS_OPT_POST_1,&os_err);
        } else if(gameState.mode == GAME_MODE_GAME_OVER){
            //gameState.player.vX = 0.0f;
            //gameState.player.vY = 0.0f;
            if(axis_data.y < FORWARD_THRESHOLD_Y){
                OSSemPost(&gyro_sem, OS_OPT_POST_1, &os_err);
            }
            
        }
        
        
        OSTimeDlyHMSM(0, 0, 0, INPUT_TASK_DELAY, OS_OPT_TIME_HMSM_STRICT, &os_err);
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

    c6dofimu14_axis_t *axis_data; //create c6dofimu14 data struct object
    c6dofimu14_axis_t old_data = {46, 46, 0}; //middle, no z
    
    CPU_INT08U prev_player[2] = {0, 0}; //initialize player position
    
    CPU_INT08U sizeFactor = 2;
    CPU_INT08U z_axis = 0;
    CPU_INT08U range = 3;
    
    //set oledc font, initialize asteroids
    oledc_set_font(guiFont_Tahoma_7_Regular,COLOR_WHITE,_OLEDC_FO_HORIZONTAL);
    Rectangle old_rectangle_frame_buffer [MAX_ASTEROIDS];
    Rectangle rectangle_frame_buffer_display [MAX_ASTEROIDS];
    
    for(CPU_INT08U i = 0; i<MAX_ASTEROIDS ; i++){
        old_rectangle_frame_buffer[i].x = -1;
        frame_buffer_copy.rectangles[i].x = -1;
    }    
    
    oledc_fill_screen(COLOR_BLACK);
    
  /* prevent compiler warnings */
    (void)p_arg;
  
  /* start of the endless loop */
  while (DEF_TRUE) {
    
    OSSemPend(&gameState_sem, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);   
    if(gameState.mode == GAME_MODE_RUNNING){
        asteroids_DrawBorder(COLOR_GREEN);
        asteroids_DrawArena(&gameState);
        
        OSSemPend(&frame_buffer_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
        for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
            rectangle_frame_buffer_display[i] = frame_buffer_copy.rectangles[i];
        }
        
        OSSemPost(&frame_buffer_sem, OS_OPT_POST_1, &os_err);
    
        for(CPU_INT08U i = 0; i<MAX_ASTEROIDS ; i++){
            if(old_rectangle_frame_buffer[i].x == -1) continue;
            delete_slivers(old_rectangle_frame_buffer[i], rectangle_frame_buffer_display[i]);
        }
        
        for(CPU_INT08U i = 0; i < MAX_ASTEROIDS ; i++){
            if(rectangle_frame_buffer_display[i].x == -1) continue;
            oledc_rectangle(rectangle_frame_buffer_display[i].x, rectangle_frame_buffer_display[i].y, rectangle_frame_buffer_display[i].x + rectangle_frame_buffer_display[i].width,
            rectangle_frame_buffer_display[i].y + rectangle_frame_buffer_display[i].height, 0xFFFF);
        }
        
        for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
            old_rectangle_frame_buffer[i] = rectangle_frame_buffer_display[i];
        }
        
        draw_player(gameState.player.x, gameState.player.y, prev_player[0], prev_player[1]); 
        prev_player[0] = gameState.player.x;
        prev_player[1] = gameState.player.y;
        
        OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        
    } else if(gameState.mode == GAME_MODE_GAME_OVER){
        oledc_fill_screen(COLOR_BLACK);
        asteroids_DrawGameOver(&gameState);
        OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
    }
    
    /* initiate scheduler */
    OSTimeDlyHMSM(0, 0, 0, DISPLAY_TASK_DELAY, 
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
    gameState.player.x = 45;
    gameState.player.y = 45;
    gameState.player.size = 7;
    /* prevent compiler warnings */
    (void)p_arg;
    
    CPU_INT32U lastScoreTick = OSTimeGet(&os_err);
    
    const CPU_INT16U SCORE_PERIOD_MS = 100;
    const CPU_INT16U LOOP_DELAY_MS = 10;

    CPU_INT32U elapsed_ms = 0;
    
    CPU_INT08U sizeFactor = 3;
    
    /* start of the endless loop */ 
    while (DEF_TRUE) {
        CPU_INT32U now = OSTimeGet(&os_err);
        
        OSSemPend(&gameState_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
        if(gameState.mode == GAME_MODE_RUNNING){
            elapsed_ms += LOOP_DELAY_MS;
            
            run_a_frame(&gameState);
            //update score in GameState
            if (elapsed_ms >= SCORE_PERIOD_MS) {
                elapsed_ms -= SCORE_PERIOD_MS;//keep remainder for accuracy
                gameState.player.score++;
            }
            
            OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        } else if(gameState.mode == GAME_MODE_GAME_OVER){
            //pend on movement
            OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &os_err);
            OSSemPend(&gyro_sem, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
            
            gameState.player.x = 45; // Reset player position
            gameState.player.y = 45; // Reset player position
            gameState.player.vX = 0.0f; // Stop all movement
            gameState.player.vY = 0.0f;
            
            gameState.player.score = 0;
            gameState.mode = GAME_MODE_RUNNING;
            elapsed_ms = 0;
            
            reset_asteroids(&gameState);
            oledc_fill_screen(COLOR_BLACK);
            
            
            OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        }
        
        /* initiate scheduler */
        OSTimeDlyHMSM(0, 0, 0, GAME_LOOP_TASK_DELAY, OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}



static void App_Task_RENDERER(void *p_arg){
    /* declare and define task local variables */
    OS_ERR os_err;
    OS_MSG_SIZE size;
    CPU_TS ts;

    Asteroid asteroids [MAX_ASTEROIDS];
   
    Rectangle rectangle_frame_buffer [MAX_ASTEROIDS];
    
    /* prevent compiler warnings */
    (void)p_arg;
    
    /* start of the endless loop */ 
    while (DEF_TRUE) {
        //wait for all writes to gameState to finish
        OSSemPend(&gameState_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);

        for(CPU_INT08U i = 0; i< MAX_ASTEROIDS; i++){
          asteroids[i] = gameState.asteroids[i];
        }
        OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        
        for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
            //Reset the rectangle buffer for this asteroid index
            rectangle_frame_buffer[i].x = -1; 
            
            if(!asteroids[i].is_active) continue;

            CPU_FP32 x_start = asteroids[i].x;
            CPU_FP32 y_start = asteroids[i].y;
            CPU_INT08U size = asteroids[i].size;
            
            //Calculate clipping: The coordinates that are actually on the screen (0 to 95)
            
            CPU_FP32 x_visible_min = (x_start < 0) ? 0 : x_start;
            CPU_FP32 y_visible_min = (y_start < 0) ? 0 : y_start;
            
            CPU_FP32 x_end = x_start + (CPU_FP32)size;
            CPU_FP32 y_end = y_start + (CPU_FP32)size;
            
            CPU_FP32 x_visible_max = (x_end > 96) ? 96 : x_end;
            CPU_FP32 y_visible_max = (y_end > 96) ? 96 : y_end;
            
            //Check if asteroid is entirely off-screen (should be caught by destroy_asteroids, 
            //fail-safe for rendering
            if (x_visible_min >= x_visible_max || y_visible_min >= y_visible_max) {
                //off-screen
                continue; 
            }
            
            //Populate the frame buffer for the visible portion
            rectangle_frame_buffer[i].x = (CPU_INT08S)round(x_visible_min);
            rectangle_frame_buffer[i].y = (CPU_INT08S)round(y_visible_min);
            //Width/Height = max_coord - min_coord
            rectangle_frame_buffer[i].width =(CPU_INT08S)round(x_visible_max - x_visible_min);
            rectangle_frame_buffer[i].height = (CPU_INT08S)round(y_visible_max - y_visible_min);
        }

        //Update Render Buffer (Write Lock)
        OSSemPend(&frame_buffer_sem, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
        
        //Copy the *entire* array to ensure inactive/deleted asteroids 
        for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
            frame_buffer_copy.rectangles[i] = rectangle_frame_buffer[i];
        }
        
        OSSemPost(&frame_buffer_sem, OS_OPT_POST_1, &os_err);
             
        /* initiate scheduler */
        OSTimeDlyHMSM(0, 0, 0, RENDERER_TASK_DELAY, OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}

/* END OF FILE */