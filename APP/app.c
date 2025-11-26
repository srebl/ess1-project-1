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


#define FORWARD_THRESHOLD_Y                         20 //game restart
#define SCORE_PERIOD_MS                             100
#define LOOP_DELAY_MS                               10
#define ASTEROID_NOT_ON_SCREEN                      -1

/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB   App_TaskStartTCB;
static  CPU_STK  App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   App_TaskINPUT_TCB;
static  CPU_STK  App_TaskINPUTStk[APP_CFG_TASK_INPUT_STK_SIZE];

static  OS_TCB   App_TaskDISPLAY_TCB;
static  CPU_STK  App_TaskDISPLAYStk[APP_CFG_TASK_DISPLAY_STK_SIZE];

static OS_TCB  App_Task_GAME_LOOP_TCB;
static CPU_STK App_Task_GAME_LOOP_Stk[APP_CFG_TASK_GAME_LOOP_STK_SIZE];

static OS_TCB  App_Task_RENDERER_TCB;
static CPU_STK App_Task_RENDERER_Stk[APP_CFG_TASK_RENDERER_STK_SIZE];

OS_Q msg_q;
/**
 * @var OS_SEM gameState_sem
 * @brief Global OS_SEM object.
 * @details Binary semaphore protecting and signaling access to the game state. 
 *          Use to serialize updates between tasks.
 */
OS_SEM gameState_sem;
/**
 * @var OS_SEM frame_buffer_sem
 * @brief Global OS_SEM object.
 * @details Binary semaphore guarding the active frame buffer. 
 *          Ensures exclusive access for the raster and display tasks.
 */
OS_SEM frame_buffer_sem;
/**
 * @var OS_SEM gyro_sem
 * @brief Global OS_SEM object.
 * @details Counting semaphore posted by the IMU/gyro ISR or acquisition task to indicate 
 *          new sensor data is ready for processing.
 */
OS_SEM gyro_sem;

/**
 * @var static GameState gameState
 * @brief Global static GameState object.
 */
static GameState gameState;

/**
 * @struct FrameBuffer
 * @brief Struct holding the asteroid coordinates and sizes aswell as player coordinates.
 */
typedef struct{
  Rectangle rectangles[MAX_ASTEROIDS];
  CPU_INT08U player_x,player_y;
}FrameBuffer;

/**
 * @var static FrameBuffer frame_buffer_copy
 * @brief Global static FrameBuffer object.
 */
static FrameBuffer frame_buffer_copy;
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart  (void *p_arg);

static  void  App_TaskCreate (void);
static  void  App_ObjCreate  (void);

static void App_TaskINPUT (void *p_arg);
static void App_TaskDISPLAY (void *p_arg);
static void App_Task_GAME_LOOP (void *p_arg);
static void App_Task_RENDERER (void *p_arg);

/**
 * @brief Standard C entry point.
 * @details This is the standard entry point for C code.  It is assumed that your code will call
 *          main() once you have performed all necessary initialization.
 *
 * @return None.
 * @note Called by the startup code.
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


/**
 * @brief Application startup task.
 * @details This is an example of a startup task.  As mentioned in the book's text, you MUST
 *          initialize the ticker only once multitasking has started.
 *
 * @param[in] p_arg Argument passed by `OSTaskCreate()`. 
 *
 * @return  1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
 *          used.  The compiler should not generate any code for this statement.
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


/**
 * @brief Create all application tasks.
 * @details Initializes and creates all tasks required by the application.
 *          Typically called once by the system startup task.
 *
 * @return None.
 * @note Called by `App_TaskStart()`.
 */
static  void  App_TaskCreate (void)
{
  /* declare and define function local variables */
  OS_ERR  os_err;

/* create SENDER task */
  OSTaskCreate((OS_TCB      *)&App_TaskINPUT_TCB,
               (CPU_CHAR    *)"TaskINPUT",
               (OS_TASK_PTR  )App_TaskINPUT, 
               (void        *)0,
               (OS_PRIO      )APP_CFG_TASK_INPUT_PRIO,
               (CPU_STK     *)&App_TaskINPUTStk[0],
               (CPU_STK_SIZE )APP_CFG_TASK_INPUT_STK_SIZE_LIMIT,
               (CPU_STK_SIZE )APP_CFG_TASK_INPUT_STK_SIZE,
               (OS_MSG_QTY   )0u,
               (OS_TICK      )0u,
               (void        *)0,
               (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR      *)&os_err);
/* create RECEIVER task */
  OSTaskCreate((OS_TCB      *)&App_TaskDISPLAY_TCB,
               (CPU_CHAR    *)"TaskDISPLAY",
               (OS_TASK_PTR  )App_TaskDISPLAY, 
               (void        *)0,
               (OS_PRIO      )APP_CFG_TASK_DISPLAY_PRIO,
               (CPU_STK     *)&App_TaskDISPLAYStk[0],
               (CPU_STK_SIZE )APP_CFG_TASK_DISPLAY_STK_SIZE_LIMIT,
               (CPU_STK_SIZE )APP_CFG_TASK_DISPLAY_STK_SIZE,
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

/**
 * @brief Create application kernel objects.
 * @details Creates the semaphores and other kernel objects required by the
 *          application, such as frame-buffer protection, game-state protection,
 *          and IMU data synchronization.
 *
 * @return None.
 * @note Called by `App_TaskStart()`.
 */
static  void  App_ObjCreate (void)
{
  /* declare and define function local variables */
  OS_ERR  os_err;
  
    // create semaphores for frame buffer and game state
    OSSemCreate(&gameState_sem,"SEM_GAME_STATE",1,&os_err);
    OSSemCreate(&frame_buffer_sem,"SEM_FRAME_BUFFER",1,&os_err);
    OSSemCreate(&gyro_sem, "SEM_GYRO", 0, &os_err);

}


/**
 * @brief Task for reading IMU acceleration data.
 * @details This task reads acceleration data from a 6DOF IMU sensor and assigns the read data to 
 *          the GameState struct. Specifically here, only the player struct of the GameState struct.
 *          Synchronization is handled through semaphores.
 *
 * @param[in] p_arg Argument passed by `OSTaskCreate()`.
 *
 * @return None.
 */
static void App_TaskINPUT (void *p_arg)
{
  OS_ERR       os_err;
  CPU_TS       ts;
  c6dofimu14_axis_t axis_data = {0, 0, 0};

  (void)p_arg;

  CPU_FP32 vX = 0.0f;
  CPU_FP32 vY = 0.0f;
  
  while (DEF_TRUE) {
    // get new axis data
    c6dofimu14_read_accel_axis(&axis_data);

    if(gameState.mode == GAME_MODE_RUNNING){
      // critical section for gameState syncronication
      OSSemPend(&gameState_sem, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
      CPU_FP32 vX = compute_velocity((CPU_FP32)axis_data.x);
      CPU_FP32 vY = compute_velocity((CPU_FP32)axis_data.y);
      
      gameState.player.vX = vX; //Assigning Float to Float (Safe)
      gameState.player.vY = vY; //Assigning Float to Float (Safe)
      OSSemPost(&gameState_sem,OS_OPT_POST_1,&os_err);
    } else if(gameState.mode == GAME_MODE_MENU||gameState.mode == GAME_MODE_GAME_OVER){
      // a new game is triggered by tilting in the Y axis over the threshold
      if(axis_data.y < FORWARD_THRESHOLD_Y){
        gameState.mode = GAME_MODE_RUNNING;
      }   
    }
    OSTimeDlyHMSM(0, 0, 0, INPUT_TASK_DELAY, OS_OPT_TIME_HMSM_STRICT, &os_err);
  }
}

/**
 * @brief Task for displaying sensor data or graphics.
 * @details This task reads acceleration data from a 6DOF IMU sensor, logs the data if debugging 
 *          is enabled, and then delays for a specified time before repeating.
 *          Actual display behavior may depend on application state and build configuration.
 *
 * @param[in] p_arg Argument passed by `OSTaskCreate()`.
 *
 * @return None.
 */
static  void  App_TaskDISPLAY (void *p_arg)
{
  /* declare and define task local variables */
  OS_ERR       os_err;
  
  OS_MSG_SIZE size;
  CPU_TS ts;
  
  // 2 arrays holding player x and y coordinates
  CPU_INT08U prev_player[2] = {0, 0}; 
  CPU_INT08U curr_player[2] = {0, 0};
  GameMode curr_state = GAME_MODE_MENU;
  // set oledc font, initialize asteroids
  oledc_set_font(guiFont_Tahoma_7_Regular,COLOR_WHITE,_OLEDC_FO_HORIZONTAL);
  // 2 arrays holding asteroid data.
  Rectangle old_rectangle_frame_buffer [MAX_ASTEROIDS];
  Rectangle rectangle_frame_buffer_display [MAX_ASTEROIDS];
  CPU_INT08U player_triangle_offset = PLAYER_SIZE / 2;
  
  for(CPU_INT08U i = 0; i<MAX_ASTEROIDS ; i++){
      old_rectangle_frame_buffer[i].x = ASTEROID_NOT_ON_SCREEN;
      frame_buffer_copy.rectangles[i].x = ASTEROID_NOT_ON_SCREEN;
  }    
    
  /* prevent compiler warnings */
    (void)p_arg;
  
  /* start of the endless loop */
  while (DEF_TRUE) 
  {
    if(gameState.mode == GAME_MODE_RUNNING){
      if(curr_state != GAME_MODE_RUNNING){
        // reset screen to black so we can successively draw new objects on the screen 
        // needed to clear the start screen
        oledc_fill_screen(COLOR_BLACK);
      }
      
      // critical section for gameState syncronication
      OSSemPend(&gameState_sem, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);  
      // add offset because for collision hitbox
      curr_player[0] = gameState.player.x + player_triangle_offset;
      curr_player[1] = gameState.player.y + player_triangle_offset;
      OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
      asteroids_DrawBorder(COLOR_GREEN);
      asteroids_DrawArena(&gameState);
      
      // critical section for frame_buffer syncronication
      OSSemPend(&frame_buffer_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
      for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
          rectangle_frame_buffer_display[i] = frame_buffer_copy.rectangles[i];
      }
      OSSemPost(&frame_buffer_sem, OS_OPT_POST_1, &os_err);
      // TODO: Why always delete slivers if we possibly already have a black screen? 
      // delete slivers
      for(CPU_INT08U i = 0; i<MAX_ASTEROIDS ; i++){
          if(old_rectangle_frame_buffer[i].x == -1) continue;
          delete_slivers(old_rectangle_frame_buffer[i], rectangle_frame_buffer_display[i]);
      }
      // draw asteroids
      for(CPU_INT08U i = 0; i < MAX_ASTEROIDS ; i++){
          if(rectangle_frame_buffer_display[i].x == -1) continue;
          oledc_rectangle
          (
            rectangle_frame_buffer_display[i].x, 
            rectangle_frame_buffer_display[i].y, 
            rectangle_frame_buffer_display[i].x + rectangle_frame_buffer_display[i].width,
            rectangle_frame_buffer_display[i].y + rectangle_frame_buffer_display[i].height, 
            0xFFFF
          );
      }

      draw_player(curr_player[0], curr_player[1], prev_player[0], prev_player[1]); 
      // copy current state
      for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
          old_rectangle_frame_buffer[i] = rectangle_frame_buffer_display[i];
      }
      prev_player[0] = curr_player[0];
      prev_player[1] = curr_player[1];
      
      curr_state = GAME_MODE_RUNNING;
        
    } else if(gameState.mode == GAME_MODE_GAME_OVER){
        if(curr_state != GAME_MODE_GAME_OVER){
          oledc_fill_screen(COLOR_BLACK);
          asteroids_DrawGameOver(&gameState);
          OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        }
        curr_state = GAME_MODE_GAME_OVER;
    }
    
    /* initiate scheduler */
    OSTimeDlyHMSM(0, 0, 0, DISPLAY_TASK_DELAY, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &os_err);
  }
}

/**
 * @brief Main game loop task.
 * @details This task reads acceleration data from a semaphore, inits the OLED C and then prints
 *          the data as coordinates on the OLED C as a pixel on an X and Y Axis, with an additional
 *          line for its acceleration on the Z axis placed onto the Y axis.
 *
 * @param[in] p_arg Argument passed by `OSTaskCreate()`.
 *
 * @return None.
 */
static void App_Task_GAME_LOOP(void *p_arg){
    /* declare and define task local variables */
    OS_ERR os_err;
    OS_MSG_SIZE size;
    CPU_TS ts;
    game_state_init(&gameState);
    /* prevent compiler warnings */
    (void)p_arg;
    
    CPU_INT32U lastScoreTick = OSTimeGet(&os_err);

    CPU_INT32U elapsed_ms = 0;
    
    /* start of the endless loop */ 
   
      while (DEF_TRUE) 
      {
        if(gameState.mode == GAME_MODE_RUNNING){
          CPU_INT32U now = OSTimeGet(&os_err);
          elapsed_ms += LOOP_DELAY_MS;
          // critical section for gameState syncronication
          // gameState is locked as long as the new state is calculated
          OSSemPend(&gameState_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
          run_a_frame(&gameState);
          // update score in GameState
          if (elapsed_ms >= SCORE_PERIOD_MS) {
              elapsed_ms -= SCORE_PERIOD_MS;// keep remainder for accuracy
              gameState.player.score++;
          }
          OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
            
        } else if(gameState.mode == GAME_MODE_GAME_OVER){          
          OSSemPend(&gameState_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
          reset_game_state(&gameState);
          OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        }
        
        /* initiate scheduler */
        OSTimeDlyHMSM(0, 0, 0, GAME_LOOP_TASK_DELAY, OS_OPT_TIME_HMSM_STRICT, &os_err);
      }
    
}

/**
 * @brief Renderer task for preparing asteroid draw data.
 * @details This task reads the current game state, computes the visible screen
 *          regions of all active asteroids, and writes the resulting rectangles
 *          into the shared render buffer used by the display task.
 *          Synchronization is handled through semaphores to ensure safe access
 *          to both the game state and the frame buffer.
 *
 * @param[in] p_arg Argument passed by `OSTaskCreate()`.
 *
 * @return None.
 */
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
    
    while (DEF_TRUE) 
    {
      //wait for all writes to gameState to finish
      if(gameState.mode == GAME_MODE_RUNNING){
        // critical section for gameState syncronication
        // wait for an updated game state and copy changes into a local variable 
        OSSemPend(&gameState_sem,0,OS_OPT_PEND_BLOCKING,&ts,&os_err);
        for(CPU_INT08U i = 0; i< MAX_ASTEROIDS; i++){
          asteroids[i] = gameState.asteroids[i];
        }
        OSSemPost(&gameState_sem, OS_OPT_POST_1, &os_err);
        
        for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
          // Reset the rectangle buffer for this asteroid index
          rectangle_frame_buffer[i].x = ASTEROID_NOT_ON_SCREEN; 
          
          if(!asteroids[i].is_active) continue;

          CPU_FP32 x_start = asteroids[i].x;
          CPU_FP32 y_start = asteroids[i].y;
          CPU_INT08U size = asteroids[i].size;
          
          // Calculate clipping: The coordinates that are actually on the screen (0 to 95)         
          CPU_FP32 x_visible_min = (x_start < 0) ? 0 : x_start;
          CPU_FP32 y_visible_min = (y_start < 0) ? 0 : y_start;
          
          CPU_FP32 x_end = x_start + (CPU_FP32)size;
          CPU_FP32 y_end = y_start + (CPU_FP32)size;
          
          CPU_FP32 x_visible_max = (x_end > 96) ? 96 : x_end;
          CPU_FP32 y_visible_max = (y_end > 96) ? 96 : y_end;
          
          // Check if asteroid is entirely off-screen (should be caught by destroy_asteroids, 
          // fail-safe for rendering)
          if (x_visible_min >= x_visible_max || y_visible_min >= y_visible_max) {
              //off-screen
              continue; 
          }
          
          // Populate the frame buffer for the visible portion
          rectangle_frame_buffer[i].x = (CPU_INT08S)round(x_visible_min);
          rectangle_frame_buffer[i].y = (CPU_INT08S)round(y_visible_min);
          // Width/Height = max_coord - min_coord
          rectangle_frame_buffer[i].width = (CPU_INT08S)round(x_visible_max - x_visible_min);
          rectangle_frame_buffer[i].height = (CPU_INT08S)round(y_visible_max - y_visible_min);
        }

        // Update Render Buffer (Write Lock)
        OSSemPend(&frame_buffer_sem, 0, OS_OPT_PEND_BLOCKING, &ts, &os_err);
        
        // Copy the *entire* array to ensure inactive/deleted asteroids 
        for(CPU_INT08U i = 0; i < MAX_ASTEROIDS; i++){
            frame_buffer_copy.rectangles[i] = rectangle_frame_buffer[i];
        }
        
        // release write lock
        OSSemPost(&frame_buffer_sem, OS_OPT_POST_1, &os_err);
      }
            
      /* initiate scheduler */
      OSTimeDlyHMSM(0, 0, 0, RENDERER_TASK_DELAY, OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}

/* END OF FILE */
