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
static GameState gameState;
typedef struct {
  CPU_INT08U x;
  CPU_INT08U y;
}Pixel;

typedef struct {
  CPU_INT08S x;
  CPU_INT08S y;
  CPU_INT08U width;
  CPU_INT08U height;
}Rectangle;

static Pixel frame_buffer [MAX_ASTEROID_SIZE * MAX_ASTEROID_SIZE * MAX_ASTEROIDS];
static Pixel old_frame_buffer [MAX_ASTEROID_SIZE * MAX_ASTEROID_SIZE * MAX_ASTEROIDS];

static Rectangle rectangle_frame_buffer [MAX_ASTEROIDS];

static Rectangle rectangle_frame_buffer_copy [MAX_ASTEROIDS];
static Rectangle rectangle_frame_buffer_display [MAX_ASTEROIDS];
static Rectangle old_rectangle_frame_buffer [MAX_ASTEROIDS];

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
    
    OSTimeDlyHMSM(0, 0, 3, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    
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
  
   OSSemCreate(&gameState_sem,"SEM_GAME_STATE",1,&os_err);
  OSSemCreate(&frame_buffer_sem,"SEM_FRAME_BUFFER",1,&os_err);
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
    
    OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &os_err);
    
  /* start of the endless loop */
  while (DEF_TRUE) {
    // axis data is generally between 0 100, but there is also a position where the sensors get the max
    c6dofimu14_read_accel_axis(&axis_data);
    
    gameState.mode = GAME_MODE_RUNNING;
    
    OSQPost(&msg_q, &axis_data, sizeof(axis_data), OS_OPT_POST_FIFO, &os_err);
    
    /* initiate scheduler */
    OSTimeDlyHMSM(0, 0, 0, 10, 
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
    
    //oledc_fill_screen(COLOR_BLACK);
    
    CPU_INT08U sizeFactor = 2;
    CPU_INT08U z_axis = 0;
    CPU_INT08U range = 3;
    oledc_set_font(guiFont_Tahoma_7_Regular,0xffff,_OLEDC_FO_HORIZONTAL);
     for(CPU_INT08U i = 0; i<MAX_ASTEROIDS ; i++){
      old_rectangle_frame_buffer[i].x = -1;
      rectangle_frame_buffer_copy[i].x = -1;
      rectangle_frame_buffer[i].x = -1;
    }
  /* prevent compiler warnings */
    (void)p_arg;
  
  /* start of the endless loop */
  while (DEF_TRUE) {
    
      OSSemPend(&frame_buffer_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
            for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
          rectangle_frame_buffer_display[i] = rectangle_frame_buffer_copy[i];
        }
     OSSemPost(&frame_buffer_sem, OS_OPT_POST_1, &os_err);
    
    
    for(CPU_INT08U i = 0; i<MAX_ASTEROIDS ; i++){
      if(old_rectangle_frame_buffer[i].x == -1) continue;
      oledc_rectangle(old_rectangle_frame_buffer[i].x, old_rectangle_frame_buffer[i].y, old_rectangle_frame_buffer[i].x + old_rectangle_frame_buffer[i].width,
      old_rectangle_frame_buffer[i].y + old_rectangle_frame_buffer[i].height, 0x0000);
    }
    for(CPU_INT08U i = 0; i<MAX_ASTEROIDS ; i++){
      if(rectangle_frame_buffer_display[i].x == -1) continue;
      oledc_rectangle(rectangle_frame_buffer_display[i].x, rectangle_frame_buffer_display[i].y, rectangle_frame_buffer_display[i].x + rectangle_frame_buffer_display[i].width,
      rectangle_frame_buffer_display[i].y + rectangle_frame_buffer_display[i].height, 0xFFFF);
    }
        for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
          old_rectangle_frame_buffer[i] = rectangle_frame_buffer_display[i];
        }
    
    if((os_err == OS_ERR_NONE) && (axis_data != NULL)){
        //oledc_rectangle(old_data.x, old_data.y, old_data.x + sizeFactor, old_data.y + sizeFactor, COLOR_BLACK);
        OSSemPend(&gameState_sem, 0, OS_OPT_PEND_NON_BLOCKING, &ts, &os_err);
        gameState.player.x = old_data.x;
        gameState.player.y = old_data.y;
        
        old_data = *axis_data;
        
        gameState.player.vX = axis_data->x;
        gameState.player.vY = axis_data->y;
        
        
        OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        
        //oledc_rectangle(old_data.x, old_data.y, old_data.x + sizeFactor, old_data.y + sizeFactor, COLOR_GREEN);
    }
    
    /* initiate scheduler */
    OSTimeDlyHMSM(0, 0, 0, 10, 
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
    
    CPU_INT32U lastScoreTick = OSTimeGet(&os_err);
    
    const CPU_INT16U SCORE_PERIOD_MS = 1000;   // 5000 ms = 5 seconds per point
    const CPU_INT16U LOOP_DELAY_MS   = 10;     // must match the delay below

    CPU_INT32U elapsed_ms = 0;
    
    oledc_fill_screen(COLOR_BLACK);
    asteroids_DrawBorder(COLOR_GREEN);
    
    CPU_INT08U sizeFactor = 3;
    
    /* start of the endless loop */ 
    while (DEF_TRUE) {
        CPU_INT32U now = OSTimeGet(&os_err);
        
        
        if(gameState.mode == GAME_MODE_RUNNING){
            elapsed_ms += LOOP_DELAY_MS;

            if (elapsed_ms >= SCORE_PERIOD_MS) {
                elapsed_ms -= SCORE_PERIOD_MS;   // keep remainder for accuracy
                gameState.player.score++;
            }
            
            asteroids_DrawArena(&gameState);
            
            //other shit
            OSSemPend(&gameState_sem, 0, OS_OPT_PEND_NON_BLOCKING, &ts, &os_err);
            oledc_rectangle(gameState.player.x, gameState.player.y, gameState.player.x + sizeFactor, gameState.player.y + sizeFactor, COLOR_BLACK);
            
            oledc_rectangle(gameState.player.vX, gameState.player.vY, gameState.player.vX + sizeFactor, gameState.player.vY + sizeFactor, COLOR_RED);
            
            OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
            
        } else if(gameState.mode == GAME_MODE_GAME_OVER){
            asteroids_DrawGameOver(&gameState);
            
            //other shit
            
            
            
            
        }
        
        
        OSSemPend(&gameState_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
        run_a_frame(&gameState);
        OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        /* initiate scheduler */
        OSTimeDlyHMSM(0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}



static void App_Task_RENDERER(void *p_arg){
    /* declare and define task local variables */
    OS_ERR os_err;
    OS_MSG_SIZE size;
    CPU_TS ts;

    Asteroid asteroids [MAX_ASTEROIDS];
    CPU_INT08U frame_buffer_counter = 0;
    
    /* prevent compiler warnings */
    (void)p_arg;
    
    /* start of the endless loop */ 
    while (DEF_TRUE) {
        frame_buffer_counter = 0;
        OSSemPend(&gameState_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
        for(CPU_INT08U i = 0; i< MAX_ASTEROIDS; i++){
          asteroids[i] = gameState.asteroids[i];
        }
        OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        /*
        CPU_INT08U frame_buffer_counter = 0;
        for(CPU_INT08U i = 0; i< MAX_ASTEROIDS; i++){
          for(CPU_INT08U j = 0; j< asteroids[i].size; j++){
            if(asteroids[i].x+ j >0 && asteroids[i].x+ j <=96) continue;
             for(CPU_INT08U k = 0; k< asteroids[i].size; k++){
              if( asteroids[i].y + k > 0 && asteroids[i].y + k <= 96 ){
                 frame_buffer[frame_buffer_counter].x = asteroids[i].x + j;
                 frame_buffer[frame_buffer_counter].y = asteroids[i].y + k;
                 frame_buffer_counter++;
              }
            }
          }
        }
        */
        for(CPU_INT08U i = 0; i< MAX_ASTEROIDS; i++){
          if(!asteroids[i].is_active) continue;
           CPU_INT08S x = asteroids[i].x;
           CPU_INT08S y = asteroids[i].y;
           CPU_INT08U size = asteroids[i].size;
          CPU_INT08U max_width = size;
          CPU_INT08U max_height = size;
          
           CPU_BOOLEAN found = false;
          // get x
          if(x < 0 ){
            if(x + size <= 0){
              x = -1;
            }else{
              // x is negative
              max_width = size + x;
              x = 0;
            }
          }else if (x > 95){
            x = -1;
          }
          // get y
          if(y < 0 ){
            if(y + size <= 0){
              y = -1;
            }else{
              // x is negative
              max_height = size + y;
              y = 0;
            }
          }else if (y > 95){
            y = -1;
          }
          
          if(x == -1 || y == -1){
            // not found asteroid (not active probably)
               rectangle_frame_buffer[frame_buffer_counter].x = -1;
            continue;
          }
          
         // if(asteroids[i].x + i < 0 || asteroids[i].x + i > 96 || asteroids[i].y + i < 0 || asteroids[i].y +i > 96) continue;
          
          // get width
          CPU_INT08U width = 0;
          for(CPU_INT08U j = 0; j< max_width; j++){
            width = j;
            if(!(x + j + 1 <= 96 && j+1 < max_width)) break;
          }
          // height
          CPU_INT08U height = 0;
          for(CPU_INT08U j = 0; j < max_height; j++){
             height = j;
             if(!(y + j + 1 <= 96 && j+1 < max_height)) break;
          }
          rectangle_frame_buffer[frame_buffer_counter].x = x;
          rectangle_frame_buffer[frame_buffer_counter].y = y;
          rectangle_frame_buffer[frame_buffer_counter].width = width;
          rectangle_frame_buffer[frame_buffer_counter].height = height;
          frame_buffer_counter++;
        }
        frame_buffer_counter = 0;
       OSSemPend(&frame_buffer_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
            for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
          rectangle_frame_buffer_copy[i] = rectangle_frame_buffer[i];
        }
        OSSemPost(&frame_buffer_sem, OS_OPT_POST_1, &os_err);
   
        // shows the end of the array
        //frame_buffer[frame_buffer_counter].x = -1;
        //frame_buffer[frame_buffer_counter].y = -1;
        
        /* initiate scheduler */
        OSTimeDlyHMSM(0, 0, 0, 20, OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}

/* END OF FILE */