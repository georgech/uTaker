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
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
*/

#define _FREE_RTOS_APPLICATION
#include "config.h"
#if defined RUN_IN_FREE_RTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

extern void fn_uTasker_main(void *);
#if defined FREE_RTOS_UART
    extern unsigned char fnGetUART_Handle(void);
    static void uart_task(void *pvParameters);
#endif
#if defined FREE_RTOS_BLINKY
    extern void fnInitialiseRedLED(void);
    extern void fnToggleRedLED(void);
    static void blinky(void *par);
#endif
static void prvSetupHardware(void);


extern void fnFreeRTOS_main(void)
{
    prvSetupHardware();                                                  // perform any hardware setup necessary (that the uTasker initialisation hasn't done)

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
    #if defined FREE_RTOS_BLINKY
    if (xTaskCreate(                                                     // FreeRTOS blinky
        blinky,                                                          // pointer to the task
        "Blinky",                                                        // task name for kernel awareness debugging
        configMINIMAL_STACK_SIZE,                                        // task stack size
        (void*)NULL,                                                     // optional task startup argument
        tskIDLE_PRIORITY,                                                // initial priority
        NULL
        ) != pdPASS) {
        _EXCEPTION("FreeRTOS failed to initialise task");
        return;                                                          // this only happens when there was a failure to initialise the task (usually not enough heap)
    }
    #endif
    #if defined FREE_RTOS_UART
    if (xTaskCreate(                                                     // FreeRTOS blinky
        uart_task,                                                       // pointer to the task
        "uart_task",                                                        // task name for kernel awareness debugging
        configMINIMAL_STACK_SIZE,                                        // task stack size
        (void*)NULL,                                                     // optional task startup argument
        (configMAX_PRIORITIES - 1),                                      // initial priority
        NULL
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
#if defined FREE_RTOS_BLINKY
    fnInitialiseRedLED();
#endif
}

#if defined FREE_RTOS_BLINKY
// Blinky task
//
static void blinky(void *par)
{
    FOREVER_LOOP() {
        fnToggleRedLED();
        vTaskDelay(750/portTICK_RATE_MS);                                // wait for 750ms in order to flash the LED at 1Hz
    }
}
#endif

#if defined FREE_RTOS_UART
static void uart_task(void *pvParameters)
{
    QUEUE_TRANSFER length = 0;
    QUEUE_HANDLE uart_handle;
    unsigned char dataByte;
    vTaskDelay(500/portTICK_RATE_MS);                                    // wait for 500ms in order to allow uTasker to configure UART interfaces
    uart_handle = fnGetUART_Handle();                                    // get the UART handle
    fnDebugMsg("FreeRTOS Output\r\n");                                   // test a UART transmission
    FOREVER_LOOP() {
        length = fnRead(uart_handle, &dataByte, 1);                      // read a byte from the DMA input buffer (returns immediately)
        if (length != 0) {                                               // if something is available
            fnDebugMsg("Echo:");                                         // echo it back
            fnWrite(uart_handle, &dataByte, 1);                          // send the byte back
            fnDebugMsg("\r\n");                                          // with termination
        }
        else {                                                           // nothing in the input buffer
            vTaskDelay(1);                                               // wait a single tick to allow other tasks to execute
        }
    }
}
#endif
#endif