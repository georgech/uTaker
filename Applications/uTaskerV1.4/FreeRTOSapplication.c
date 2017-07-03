/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET
    
    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland
    
    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      FreeRTOSapplication.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
*/

#define _FREE_RTOS_APPLICATION
#include "config.h"
#if defined RUN_IN_FREE_RTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

extern void fn_uTasker_main(void *);
#if defined BLINKY
    extern void fnInitialiseRedLED(void); 
    extern void fnToggleRedLED(void);

    static void blinky(void *par);
#endif
static void prvSetupHardware(void);


extern void fnFreeRTOS_main(void)
{
    prvSetupHardware();                                                  // perform any hardware setup necesary (that the uTasker initialisation hasn't done)

    // Add the uTasker OS and project as a single task under FreeRTOS
    //
    if (xTaskCreate(                                                     // start uTasker as a task in the FreeRTOS task environment
        fn_uTasker_main,                                                 // pointer to the task
        "uTasker",                                                       // task name for kernel awareness debugging
        (UTASKER_STACK_SIZE / sizeof(portSTACK_TYPE)),                   // task stack size
        (void *)0,                                                       // optional task startup argument
        (UTASKER_TASK_PRIORITY),                                         // initial priority
        0                                                                // optional task handle to create
    ) != pdPASS) {
        _EXCEPTION("FreeRTOS failed to initialise task");
        return;                                                          // this only happens when there was a failure to initialise the task (usually not enough heap)
    }

    // Add further user tasks here
    //
    #if defined BLINKY                                                   // if the blinky project is defined add a task flashing an LED
    if (xTaskCreate(
        blinky,                                                          // pointer to the task
        "Blinky",                                                        // task name for kernel awareness debugging
        configMINIMAL_STACK_SIZE,                                        // task stack size
        (void*)NULL,                                                     // optional task startup argument
        tskIDLE_PRIORITY,                                                // initial priority
        0
        ) != pdPASS) {
        _EXCEPTION("FreeRTOS failed to initialise task");
        return;                                                          // this only happens when there was a failure to initialise the task (usually not enough heap)
    }
    #endif

    vTaskStartScheduler();                                               // start the created tasks for scheduling (this never returns as long as there was no error)
    
    // This never returns under normal circumstances
    //
    _EXCEPTION("FreeRTOS failed to start");
    return;                                                              // this only happens when there was a failure to initialise and start FreeRTOS (usually not enough heap)
}

static void prvSetupHardware(void)
{
    // Add FreeRTOS context hardware initialisation if required
    //
    #if defined BLINKY
    fnInitialiseRedLED();
    #endif
}

    #if defined BLINKY
// Blinky task
//
static void blinky(void *par)
{
    while ((int)1 != (int)0) {
        fnToggleRedLED();
        vTaskDelay(500/portTICK_RATE_MS);                                // wait for 500ms in order to flash the LED at 1Hz
    }
}
    #endif
#endif