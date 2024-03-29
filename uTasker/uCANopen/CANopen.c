/*
 * Main CANopen stack file. It combines Object dictionary (CO_OD) and all other
 * CANopen source files. Configuration information are read from CO_OD.h file.
 *
 * @file        CANopen.c
 * @ingroup     CO_CANopen
 * @author      Janez Paternoster
 * @copyright   2010 - 2015 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free and open source software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Following clarification and special exception to the GNU General Public
 * License is included to the distribution terms of CANopenNode:
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library. Thus, the terms and
 * conditions of the GNU General Public License cover the whole combination.
 *
 * As a special exception, the copyright holders of this library give
 * you permission to link this library with independent modules to
 * produce an executable, regardless of the license terms of these
 * independent modules, and to copy and distribute the resulting
 * executable under terms of your choice, provided that you also meet,
 * for each linked independent module, the terms and conditions of the
 * license of that module. An independent module is a module which is
 * not derived from or based on this library. If you modify this
 * library, you may extend this exception to your version of the
 * library, but you are not obliged to do so. If you do not wish
 * to do so, delete this exception statement from your version.
 */

#include "config.h"

#if defined CAN_INTERFACE && defined SUPPORT_CANopen
#include "CANopen.h"

/* If defined, global variables will be used, otherwise CANopen objects will
   be generated with _calloc(). */
/* #define CO_USE_GLOBALS */

/* If defined, the user provides an own implemetation for calculating the
 * CRC16 CCITT checksum. */
/* #define CO_USE_OWN_CRC16 */


typedef unsigned short    MAX_MALLOC;                                    // up to 64k heap chunks
extern void *uMalloc(MAX_MALLOC);
#define _calloc(a,b) uMalloc(a*b)

/* Global variables ***********************************************************/
    extern const CO_OD_entry_t CO_OD[CO_OD_NoOfElements];  /* Object Dictionary array */
    CO_t *CO[CANOPEN_INSTANCES] = {0};

    static CO_CANrx_t          *CO_CANmodule_rxArray0;
    static CO_CANtx_t          *CO_CANmodule_txArray0;
    static CO_OD_extension_t   *CO_SDO_ODExtensions;
    static CO_HBconsNode_t     *CO_HBcons_monitoredNodes;
#if CO_NO_TRACE > 0
    static uint32_t            *CO_traceTimeBuffers[CO_NO_TRACE];
    static int32_t             *CO_traceValueBuffers[CO_NO_TRACE];
#endif


/* Verify features from CO_OD *************************************************/
    /* generate error, if features are not correctly configured for this project */
    #if        CO_NO_NMT_MASTER                           >  1     \
            || CO_NO_SYNC                                 != 1     \
            || CO_NO_EMERGENCY                            != 1     \
            || CO_NO_SDO_SERVER                           == 0     \
            || (CO_NO_SDO_CLIENT != 0 && CO_NO_SDO_CLIENT != 1)    \
            || (CO_NO_RPDO < 1 || CO_NO_RPDO > 0x200)              \
            || (CO_NO_TPDO < 1 || CO_NO_TPDO > 0x200)              \
            || ODL_consumerHeartbeatTime_arrayLength      == 0     \
            || ODL_errorStatusBits_stringLength           < 10
        #error Features from CO_OD.h file are not corectly configured for this project!
    #endif


/* Indexes for CANopenNode message objects ************************************/
    #ifdef ODL_consumerHeartbeatTime_arrayLength
        #define CO_NO_HB_CONS   ODL_consumerHeartbeatTime_arrayLength
    #else
        #define CO_NO_HB_CONS   0
    #endif

    #define CO_RXCAN_NMT       0                                      /*  index for NMT message */
    #define CO_RXCAN_SYNC      1                                      /*  index for SYNC message */
    #define CO_RXCAN_RPDO     (CO_RXCAN_SYNC+CO_NO_SYNC)              /*  start index for RPDO messages */
    #define CO_RXCAN_SDO_SRV  (CO_RXCAN_RPDO+CO_NO_RPDO)              /*  start index for SDO server message (request) */
    #define CO_RXCAN_SDO_CLI  (CO_RXCAN_SDO_SRV+CO_NO_SDO_SERVER)     /*  start index for SDO client message (response) */
    #define CO_RXCAN_CONS_HB  (CO_RXCAN_SDO_CLI+CO_NO_SDO_CLIENT)     /*  start index for Heartbeat Consumer messages */
    /* total number of received CAN messages */
    #define CO_RXCAN_NO_MSGS (1+CO_NO_SYNC+CO_NO_RPDO+CO_NO_SDO_SERVER+CO_NO_SDO_CLIENT+CO_NO_HB_CONS)

    #define CO_TXCAN_NMT       0                                      /*  index for NMT master message */
    #define CO_TXCAN_SYNC      CO_TXCAN_NMT+CO_NO_NMT_MASTER          /*  index for SYNC message */
    #define CO_TXCAN_EMERG    (CO_TXCAN_SYNC+CO_NO_SYNC)              /*  index for Emergency message */
    #define CO_TXCAN_TPDO     (CO_TXCAN_EMERG+CO_NO_EMERGENCY)        /*  start index for TPDO messages */
    #define CO_TXCAN_SDO_SRV  (CO_TXCAN_TPDO+CO_NO_TPDO)              /*  start index for SDO server message (response) */
    #define CO_TXCAN_SDO_CLI  (CO_TXCAN_SDO_SRV+CO_NO_SDO_SERVER)     /*  start index for SDO client message (request) */
    #define CO_TXCAN_HB       (CO_TXCAN_SDO_CLI+CO_NO_SDO_CLIENT)     /*  index for Heartbeat message */
    /* total number of transmitted CAN messages */
    #define CO_TXCAN_NO_MSGS (CO_NO_NMT_MASTER+CO_NO_SYNC+CO_NO_EMERGENCY+CO_NO_TPDO+CO_NO_SDO_SERVER+CO_NO_SDO_CLIENT+1)





/* Helper function for NMT master *********************************************/
#if CO_NO_NMT_MASTER == 1
    CO_CANtx_t *NMTM_txBuff = 0;

    uint8_t CO_sendNMTcommand(CO_t *CO, uint8_t command, uint8_t nodeID){
        if(NMTM_txBuff == 0){
            /* error, CO_CANtxBufferInit() was not called for this buffer. */
            return CO_ERROR_TX_UNCONFIGURED; /* -11 */
        }
        NMTM_txBuff->data[0] = command;
        NMTM_txBuff->data[1] = nodeID;

        /* Apply NMT command also to this node, if set so. */
        if(nodeID == 0 || nodeID == CO->NMT->nodeId){
            switch(command){
                case CO_NMT_ENTER_OPERATIONAL:
                    if((*CO->NMT->emPr->errorRegister) == 0) {
                        CO->NMT->operatingState = CO_NMT_OPERATIONAL;
                    }
                    break;
                case CO_NMT_ENTER_STOPPED:
                    CO->NMT->operatingState = CO_NMT_STOPPED;
                    break;
                case CO_NMT_ENTER_PRE_OPERATIONAL:
                    CO->NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
                    break;
                case CO_NMT_RESET_NODE:
                    CO->NMT->resetCommand = CO_RESET_APP;
                    break;
                case CO_NMT_RESET_COMMUNICATION:
                    CO->NMT->resetCommand = CO_RESET_COMM;
                    break;
            }
        }

        return CO_CANsend(CO->CANmodule[0], NMTM_txBuff); /* 0 = success */
    }
#endif


/******************************************************************************/
CO_t *CO_init(uint8_t instance, uint8_t nodeId, QUEUE_HANDLE CAN_interface_ID)
{
    int16_t i;
#if CO_NO_TRACE > 0
    uint32_t CO_traceBufferSize[CO_NO_TRACE];
#endif
    CO_t *ptrCO_instance = CO[instance];
    HEAP_REQUIREMENTS CAN_heap = fnHeapFree();                           // the amount of heap free before CANopen is initialised
#if defined _WINDOWS
    /* Verify parameters from CO_OD */
    if (sizeof(OD_TPDOCommunicationParameter_t) != sizeof(CO_TPDOCommPar_t)
       || sizeof(OD_TPDOMappingParameter_t) != sizeof(CO_TPDOMapPar_t)
       || sizeof(OD_RPDOCommunicationParameter_t) != sizeof(CO_RPDOCommPar_t)
       || sizeof(OD_RPDOMappingParameter_t) != sizeof(CO_RPDOMapPar_t)) {
        return 0;
    }

    #if CO_NO_SDO_CLIENT == 1
    if (sizeof(OD_SDOClientParameter_t) != sizeof(CO_SDOclientPar_t)) {
        return 0;
    }
    #endif
#endif
    /* Verify CANopen Node-ID */
    if ((nodeId < 1) || (nodeId > 127)) {
        return 0;
    }

    // Initialize CANopen object
    //
    if (ptrCO_instance == NULL) {
        ptrCO_instance = CO[instance] = (CO_t *)uMalloc(sizeof(CO_t));
        ptrCO_instance->CANmodule[0]        = (CO_CANmodule_t *)    _calloc(1, sizeof(CO_CANmodule_t));
        CO_CANmodule_rxArray0               = (CO_CANrx_t *)        _calloc(CO_RXCAN_NO_MSGS, sizeof(CO_CANrx_t));
        CO_CANmodule_txArray0               = (CO_CANtx_t *)        _calloc(CO_TXCAN_NO_MSGS, sizeof(CO_CANtx_t));
        for (i = 0; i < CO_NO_SDO_SERVER; i++) {
            ptrCO_instance->SDO[i]          = (CO_SDO_t *)          _calloc(1, sizeof(CO_SDO_t));
        }
        CO_SDO_ODExtensions                 = (CO_OD_extension_t*)  _calloc(CO_OD_NoOfElements, sizeof(CO_OD_extension_t));
        ptrCO_instance->em                  = (CO_EM_t *)           _calloc(1, sizeof(CO_EM_t));
        ptrCO_instance->emPr                = (CO_EMpr_t *)         _calloc(1, sizeof(CO_EMpr_t));
        ptrCO_instance->NMT                 = (CO_NMT_t *)          _calloc(1, sizeof(CO_NMT_t));
        ptrCO_instance->SYNC                = (CO_SYNC_t *)         _calloc(1, sizeof(CO_SYNC_t));
        for (i = 0; i < CO_NO_RPDO; i++) {
            ptrCO_instance->RPDO[i]         = (CO_RPDO_t *)         _calloc(1, sizeof(CO_RPDO_t));
        }
        for (i = 0; i < CO_NO_TPDO; i++) {
            ptrCO_instance->TPDO[i]         = (CO_TPDO_t *)         _calloc(1, sizeof(CO_TPDO_t));
        }
        ptrCO_instance->HBcons              = (CO_HBconsumer_t *)   _calloc(1, sizeof(CO_HBconsumer_t));
        CO_HBcons_monitoredNodes            = (CO_HBconsNode_t *)   _calloc(CO_NO_HB_CONS, sizeof(CO_HBconsNode_t));
      #if CO_NO_SDO_CLIENT == 1
        ptrCO_instance->SDOclient           = (CO_SDOclient_t *)    _calloc(1, sizeof(CO_SDOclient_t));
      #endif
      #if CO_NO_TRACE > 0
        for (i = 0; i < CO_NO_TRACE; i++) {
            ptrCO_instance->trace[i]        = (CO_trace_t *)        _calloc(1, sizeof(CO_trace_t));
            CO_traceTimeBuffers[i]          = (uint32_t *)          _calloc(OD_traceConfig[i].size, sizeof(uint32_t));
            CO_traceValueBuffers[i]         = (int32_t *)           _calloc(OD_traceConfig[i].size, sizeof(int32_t));
            if ((CO_traceTimeBuffers[i] != NULL) && (CO_traceValueBuffers[i] != NULL)) {
                CO_traceBufferSize[i] = OD_traceConfig[i].size;
            }
            else {
                CO_traceBufferSize[i] = 0;
            }
        }
      #endif
    }
    CAN_heap -= fnHeapFree();                                            // the amount of heap allocated for CANopen use

    ptrCO_instance->CANmodule[0]->CANmsgBuffSize = NUMBER_CAN_MESSAGE_BUFFERS;
    ptrCO_instance->CANmodule[0]->rxArray = CO_CANmodule_rxArray0;
    ptrCO_instance->CANmodule[0]->rxSize = CO_RXCAN_NO_MSGS;
    ptrCO_instance->CANmodule[0]->txArray = CO_CANmodule_txArray0;
    ptrCO_instance->CANmodule[0]->txSize = CO_TXCAN_NO_MSGS;
    ptrCO_instance->CANmodule[0]->firstCANtxMessage = 1;
    ptrCO_instance->CANmodule[0]->CAN_interface_ID = CAN_interface_ID;

    for (i = 0; i < CO_NO_SDO_SERVER; i++) {
        uint32_t COB_IDClientToServer;
        uint32_t COB_IDServerToClient;
        if (i == 0) {
            // Default SDO server must be located at first index
            //
            COB_IDClientToServer = (CO_CAN_ID_RSDO + nodeId);
            COB_IDServerToClient = (CO_CAN_ID_TSDO + nodeId);
        }
        else {
            COB_IDClientToServer = OD_SDOServerParameter[i].COB_IDClientToServer;
            COB_IDServerToClient = OD_SDOServerParameter[i].COB_IDServerToClient;
        }

        CO_SDO_init(
            ptrCO_instance->SDO[i],
                COB_IDClientToServer,
                COB_IDServerToClient,
                OD_H1200_SDO_SERVER_PARAM+i,
                i==0 ? 0 : ptrCO_instance->SDO[0],
               &CO_OD[0],
                CO_OD_NoOfElements,
                CO_SDO_ODExtensions,
                nodeId,
            ptrCO_instance->CANmodule[0],
                CO_RXCAN_SDO_SRV+i,
            ptrCO_instance->CANmodule[0],
                CO_TXCAN_SDO_SRV+i);
    }

    // Initialise emergency object
    //
    CO_EM_init(
        ptrCO_instance->em,
        ptrCO_instance->emPr,
        ptrCO_instance->SDO[0],
           &OD_errorStatusBits[0],
            ODL_errorStatusBits_stringLength,
           &OD_errorRegister,
           &OD_preDefinedErrorField[0],
            ODL_preDefinedErrorField_arrayLength,
        ptrCO_instance->CANmodule[0],
            CO_TXCAN_EMERG,
            CO_CAN_ID_EMERGENCY + nodeId);

    // Initialise hearthbeat object
    //
    CO_NMT_init(
        ptrCO_instance->NMT,
        ptrCO_instance->emPr,
            nodeId,
            500,
        ptrCO_instance->CANmodule[0],
            CO_RXCAN_NMT,
            CO_CAN_ID_NMT_SERVICE,
        ptrCO_instance->CANmodule[0],
            CO_TXCAN_HB,
            CO_CAN_ID_HEARTBEAT + nodeId);


#if CO_NO_NMT_MASTER == 1
    NMTM_txBuff = CO_CANtxBufferInit(/* return pointer to 8-byte CAN data buffer, which should be populated */
        ptrCO_instance->CANmodule[0], /* pointer to CAN module used for sending this message */
            CO_TXCAN_NMT,     /* index of specific buffer inside CAN module */
            0x0000,           /* CAN identifier */
            0,                /* rtr */
            2,                /* number of data bytes */
            0);               /* synchronous message flag bit */
#endif

    // Initialise synch object
    //
    CO_SYNC_init(
        ptrCO_instance->SYNC,
        ptrCO_instance->em,
        ptrCO_instance->SDO[0],
           &ptrCO_instance->NMT->operatingState,
            OD_COB_ID_SYNCMessage,
            OD_communicationCyclePeriod,
            OD_synchronousCounterOverflowValue,
        ptrCO_instance->CANmodule[0],
            CO_RXCAN_SYNC,
        ptrCO_instance->CANmodule[0],
            CO_TXCAN_SYNC);

    // Initialise process data objects (reception)
    //
    for (i = 0; i < CO_NO_RPDO; i++) {
        CO_CANmodule_t *CANdevRx = ptrCO_instance->CANmodule[0];
        uint16_t CANdevRxIdx = (CO_RXCAN_RPDO + i);
        CO_RPDO_init(
            ptrCO_instance->RPDO[i],
            ptrCO_instance->em,
            ptrCO_instance->SDO[0],
            ptrCO_instance->SYNC,
               &ptrCO_instance->NMT->operatingState,
                nodeId,
                ((i<4) ? (CO_CAN_ID_RPDO_1+i*0x100) : 0),
                0,
                (CO_RPDOCommPar_t*) &OD_RPDOCommunicationParameter[i],
                (CO_RPDOMapPar_t*) &OD_RPDOMappingParameter[i],
                (OD_H1400_RXPDO_1_PARAM + i),
                (OD_H1600_RXPDO_1_MAPPING + i),
                CANdevRx,
                CANdevRxIdx);
    }

    // Initialise process data objects (transmission)
    //
    for (i = 0; i < CO_NO_TPDO; i++) {
        CO_TPDO_init(
            ptrCO_instance->TPDO[i],
            ptrCO_instance->em,
            ptrCO_instance->SDO[0],
               &ptrCO_instance->NMT->operatingState,
                nodeId,
                ((i<4) ? (CO_CAN_ID_TPDO_1+i*0x100) : 0),
                0,
                (CO_TPDOCommPar_t*) &OD_TPDOCommunicationParameter[i],
                (CO_TPDOMapPar_t*) &OD_TPDOMappingParameter[i],
                OD_H1800_TXPDO_1_PARAM+i,
                OD_H1A00_TXPDO_1_MAPPING+i,
            ptrCO_instance->CANmodule[0],
                CO_TXCAN_TPDO+i);
    }

    CO_HBconsumer_init(
        ptrCO_instance->HBcons,
        ptrCO_instance->em,
        ptrCO_instance->SDO[0],
           &OD_consumerHeartbeatTime[0],
            CO_HBcons_monitoredNodes,
            CO_NO_HB_CONS,
        ptrCO_instance->CANmodule[0],
            CO_RXCAN_CONS_HB);

#if CO_NO_SDO_CLIENT == 1
    CO_SDOclient_init(
        ptrCO_instance->SDOclient,
        ptrCO_instance->SDO[0],
            (CO_SDOclientPar_t*) &OD_SDOClientParameter[0],
        ptrCO_instance->CANmodule[0],
            CO_RXCAN_SDO_CLI,
        ptrCO_instance->CANmodule[0],
            CO_TXCAN_SDO_CLI);
#endif

#if CO_NO_TRACE > 0
    for(i=0; i<CO_NO_TRACE; i++) {
        CO_trace_init(
            ptrCO_instance->trace[i],
            ptrCO_instance->SDO[0],
            OD_traceConfig[i].axisNo,
            CO_traceTimeBuffers[i],
            CO_traceValueBuffers[i],
            CO_traceBufferSize[i],
            &OD_traceConfig[i].map,
            &OD_traceConfig[i].format,
            &OD_traceConfig[i].trigger,
            &OD_traceConfig[i].threshold,
            &OD_trace[i].value,
            &OD_trace[i].min,
            &OD_trace[i].max,
            &OD_trace[i].triggerTime,
            OD_INDEX_TRACE_CONFIG + i,
            OD_INDEX_TRACE + i);
    }
#endif
    return ptrCO_instance;
}

extern int uCANopenInit(int iInstance, QUEUE_HANDLE CAN_interface_ID, unsigned char ucNodeID)
{
    CO_t *ptrCO_instance = CO_init((unsigned char)iInstance, ucNodeID, CAN_interface_ID);
    if (ptrCO_instance != 0) {
        CO_CANsetNormalMode(ptrCO_instance->CANmodule[0]);
        return 0;
    }
    return -1;
}


/******************************************************************************/
void CO_delete(int32_t CANbaseAddress){
#if 0
#ifndef CO_USE_GLOBALS
    int16_t i;
#endif

    CO_CANsetConfigurationMode(CANbaseAddress);
    CO_CANmodule_disable(CO->CANmodule[0]);

#ifndef CO_USE_GLOBALS
  #if CO_NO_TRACE > 0
      for(i=0; i<CO_NO_TRACE; i++) {
          free(CO->trace[i]);
          free(CO_traceTimeBuffers[i]);
          free(CO_traceValueBuffers[i]);
      }
  #endif
  #if CO_NO_SDO_CLIENT == 1
    free(CO->SDOclient);
  #endif
    free(CO_HBcons_monitoredNodes);
    free(CO->HBcons);
    for(i=0; i<CO_NO_RPDO; i++){
        free(CO->RPDO[i]);
    }
    for(i=0; i<CO_NO_TPDO; i++){
        free(CO->TPDO[i]);
    }
    free(CO->SYNC);
    free(CO->NMT);
    free(CO->emPr);
    free(CO->em);
    free(CO_SDO_ODExtensions);
    for(i=0; i<CO_NO_SDO_SERVER; i++){
        free(CO->SDO[i]);
    }
    free(CO_CANmodule_txArray0);
    free(CO_CANmodule_rxArray0);
    free(CO->CANmodule[0]);
    CO = NULL;
#endif
#endif
}


/******************************************************************************/
CO_NMT_reset_cmd_t CO_process(
        CO_t                   *CO,
        uint16_t                timeDifference_ms,
        uint16_t               *timerNext_ms)
{
    uint8_t i;
    bool_t NMTisPreOrOperational = false;
    CO_NMT_reset_cmd_t reset = CO_RESET_NOT;
    static uint16_t ms50 = 0;

    if(CO->NMT->operatingState == CO_NMT_PRE_OPERATIONAL || CO->NMT->operatingState == CO_NMT_OPERATIONAL)
        NMTisPreOrOperational = true;

    ms50 += timeDifference_ms;
    if(ms50 >= 50){
        ms50 -= 50;
        CO_NMT_blinkingProcess50ms(CO->NMT);
    }
    if(timerNext_ms != NULL){
        if(*timerNext_ms > 50){
            *timerNext_ms = 50;
        }
    }


    for(i=0; i<CO_NO_SDO_SERVER; i++){
        CO_SDO_process(
                CO->SDO[i],
                NMTisPreOrOperational,
                timeDifference_ms,
                1000,
                timerNext_ms);
    }

    CO_EM_process(
            CO->emPr,
            NMTisPreOrOperational,
            timeDifference_ms * 10,
            OD_inhibitTimeEMCY);


    reset = CO_NMT_process(
            CO->NMT,
            timeDifference_ms,
            OD_producerHeartbeatTime,
            OD_NMTStartup,
            OD_errorRegister,
            OD_errorBehavior,
            timerNext_ms);


    CO_HBconsumer_process(
            CO->HBcons,
            NMTisPreOrOperational,
            timeDifference_ms);

    return reset;
}


/******************************************************************************/
bool_t CO_process_SYNC_RPDO(
        CO_t                   *CO,
        uint32_t                timeDifference_us)
{
    int16_t i;
    bool_t syncWas = false;

    switch (CO_SYNC_process(CO->SYNC, timeDifference_us, OD_synchronousWindowLength)) {
    case 1:     //immediately after the SYNC message
        syncWas = true;
        break;
    case 2:     //outside SYNC window
        CO_CANclearPendingSyncPDOs(CO->CANmodule[0]);
        break;
    }

    for (i = 0; i < CO_NO_RPDO; i++) {
        CO_RPDO_process(CO->RPDO[i], syncWas);
    }
    return syncWas;
}


/******************************************************************************/
void CO_process_TPDO(CO_t *CO, bool_t syncWas, uint32_t timeDifference_us)
{
    int16_t i;

    for (i = 0; i < CO_NO_TPDO; i++) {                                   // for each possile PDO
        if (CO->TPDO[i]->sendRequest == 0) {                             // if no transmission is presently being requested
            CO->TPDO[i]->sendRequest = CO_TPDOisCOS(CO->TPDO[i]);        // verify if TPDO value has changed and request transmission if it has
        }
        CO_TPDO_process(CO->TPDO[i], CO->SYNC, syncWas, timeDifference_us);
    }
}


#define TMR_TASK_INTERVAL   (1000)          /* Interval of tmrTask thread in microseconds */

extern int uCANopenPoll(int iInstance, unsigned long CO_timer1ms)
{
    static unsigned long ulTimer1msPrevious = 0;
    CO_t *ptrCO_instance = CO[iInstance];
    unsigned long ulTimer1msCopy;
    unsigned short usTimer1msDiff;
    ulTimer1msCopy = CO_timer1ms;
    usTimer1msDiff = (unsigned short)(ulTimer1msCopy - ulTimer1msPrevious); // the number of ms elapsed since last poll
    ulTimer1msPrevious = ulTimer1msCopy;
    return (CO_process(ptrCO_instance, usTimer1msDiff, NULL));           // CANopen process
}

/* timer thread executes in constant intervals ********************************/
extern void tmrTask_thread(int iInstance)
{
    CO_t *ptrCO_instance = CO[iInstance];
    if (ptrCO_instance->CANmodule[0]->CANnormal != 0) {
        bool_t syncWas;
        syncWas = CO_process_SYNC_RPDO(ptrCO_instance, TMR_TASK_INTERVAL); // process sync and read inputs

        /* Further I/O or nonblocking application code may go here. */

        CO_process_TPDO(ptrCO_instance, syncWas, TMR_TASK_INTERVAL);     // write outputs

        if (0) {                                                         // verify timer overflow
            CO_errorReport(ptrCO_instance->em, CO_EM_ISR_TIMER_OVERFLOW, CO_EMC_SOFTWARE_INTERNAL, 0U);
        }
    }
}
#endif
