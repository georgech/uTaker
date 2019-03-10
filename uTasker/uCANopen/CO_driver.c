/*
 * CAN module object for Freescale MCF5282 ColdFire V2 microcontroller.
 *
 * @file        CO_driver.c
 * @author      Janez Paternoster
 * @author      Laurent Grosbois
 * @copyright   2004 - 2012 Janez Paternoster
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
#include "CO_driver.h"
#include "CO_Emergency.h"


extern const CO_CANbitRateData_t  CO_CANbitRateData[8];
extern CO_t *CO[CANOPEN_INSTANCES];

#define REMOTE_FRAME_FLAG    0x8000


/******************************************************************************/
void CO_CANsetConfigurationMode(uint16_t CANbaseAddress){
#if 0
    /* sets the module as running */
    MCF_FlexCAN_CANMCR &= ~MCF_FlexCAN_CANMCR_STOP;

    /* enter debug mode */
    MCF_FlexCAN_CANMCR |= MCF_FlexCAN_CANMCR_FRZ | MCF_FlexCAN_CANMCR_HALT;

    /* wait for entering in the mode */
    while(!(MCF_FlexCAN_CANMCR&MCF_FlexCAN_CANMCR_FRZACK)){};
#endif
}


/******************************************************************************/
void CO_CANsetNormalMode(CO_CANmodule_t *CANmodule){
#if 0
    /* sets the module as running & exit debug mode */
    MCF_FlexCAN_CANMCR &= ~MCF_FlexCAN_CANMCR_STOP & ~MCF_FlexCAN_CANMCR_FRZ & ~MCF_FlexCAN_CANMCR_HALT;

    /* wait for entering in the mode */
    while(!(MCF_FlexCAN_CANMCR&&MCF_FlexCAN_CANMCR_NOTRDY)){};
#endif
    CANmodule->CANnormal = true;
}

#if 0
/******************************************************************************/
CO_ReturnError_t CO_CANmodule_init(
        CO_CANmodule_t         *CANmodule,
        uint16_t                CANbaseAddress,
        CO_CANrx_t              rxArray[],
        uint16_t                rxSize,
        CO_CANtx_t              txArray[],
        uint16_t                txSize,
        uint16_t                CANbitRate)
{
    uint16_t i;
    uint8_t nb_CANbuff = 16; //16 CAN buffers

    /* verify arguments */
    if(CANmodule==NULL || rxArray==NULL || txArray==NULL){
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Configure object variables */
    CANmodule->CANbaseAddress = CANbaseAddress;
    CANmodule->CANmsgBuffSize = nb_CANbuff;
    CANmodule->rxArray = rxArray;
    CANmodule->rxSize = rxSize;
    CANmodule->txArray = txArray;
    CANmodule->txSize = txSize;
    CANmodule->CANnormal = false;
    CANmodule->useCANrxFilters = false; //no filters or ((rxSize <= xx) ? true : false;)
    CANmodule->bufferInhibitFlag = false;
    CANmodule->firstCANtxMessage = true;
    CANmodule->CANtxCount = 0U;
    CANmodule->errOld = 0U;
    CANmodule->em = NULL;

    for(i=0U; i<rxSize; i++){
        rxArray[i].ident = 0U;
        rxArray[i].pFunct = NULL;
    }
    for(i=0U; i<txSize; i++){
        txArray[i].bufferFull = false;
    }

    /**** HARDWARE CONFIGURATION ****/

    /* soft reset */
    MCF_FlexCAN_CANMCR |= MCF_FlexCAN_CANMCR_SOFTRST;

    /* Tx & Rx pin modes */
    MCF_FlexCAN_CANCTRL0 &= ~MCF_FlexCAN_CANCTRL0_BOFFMSK & ~MCF_FlexCAN_CANCTRL0_ERRMSK  & ~MCF_FlexCAN_CANCTRL0_RXMODE  & ~MCF_FlexCAN_CANCTRL0_TXMODE(0);
                        /*  - no Bus off interrupt
                            - no error interrupt
                            - Rx mode : 0 is dominant bit
                            - Tx mode : full CMOS positive */

    //reset ctrl registers
    MCF_FlexCAN_CANCTRL1 = 0x00;
    MCF_FlexCAN_CANCTRL2 = 0x00;

    /* Configure CAN timing */
    switch(CANbitRate){
        default:
        case 125:  i=3; break;
        case 1000: i=7; break;
    }

    MCF_FlexCAN_CANCTRL1 |= MCF_FlexCAN_CANCTRL1_PROPSEG(CO_CANbitRateData[i].PROP);
    MCF_FlexCAN_CANCTRL2 |= MCF_FlexCAN_CANCTRL2_RJW(CO_CANbitRateData[i].SJW);
    MCF_FlexCAN_CANCTRL2 |= MCF_FlexCAN_CANCTRL2_PSEG1(CO_CANbitRateData[i].phSeg1);
    MCF_FlexCAN_CANCTRL2 |= MCF_FlexCAN_CANCTRL2_PSEG2(CO_CANbitRateData[i].phSeg2);
    MCF_FlexCAN_PRESDIV = CO_CANbitRateData[i].BRP;

    /*** here should go specific option concerning CTRL1 & CTRL2 registers ***/


    /* CAN module hardware filters */
    //clear all filter control registers
    MCF_FlexCAN_RXGMASK  = 0x00; //mask for Rx[0-13]
    MCF_FlexCAN_RX14MASK = 0x00; //mask for Rx14
    MCF_FlexCAN_RX15MASK = 0x00; //mask for Rx15


    //Set masks to accept all all messages with standard 11-bit identifier
    MCF_FlexCAN_RXGMASK  = 0x00080000;
    MCF_FlexCAN_RX14MASK = 0x00080000;
    MCF_FlexCAN_RX15MASK = 0x00080000;


    /*
    if(CANmodule->useCANrxFilters){
        //CAN module filters are used, they will be configured with
        //CO_CANrxBufferInit() functions, called by separate CANopen
        //init functions.
        //Configure all masks so, that received message must match filter
        MCF_FlexCAN_RXGMASK  = 0xFFE80000;
        MCF_FlexCAN_RX14MASK = 0xFFE80000;
        MCF_FlexCAN_RX15MASK = 0xFFE80000;
    }
    else{
        //CAN module filters are not used, all messages with standard 11-bit
        //identifier will be received
        //Configure all mask so, that all messages with standard identifier are accepted
        MCF_FlexCAN_RXGMASK  = 0x00080000;
        MCF_FlexCAN_RX14MASK = 0x00080000;
        MCF_FlexCAN_RX15MASK = 0x00080000;
    }*/


    /* CAN Module configuration register */
    MCF_FlexCAN_CANMCR &= ~MCF_FlexCAN_CANMCR_STOP & ~MCF_FlexCAN_CANMCR_FRZ & ~MCF_FlexCAN_CANMCR_HALT;


    /* configure buffers 0-13 as input buffers */
    for(i=0;i<14;i++)
    {
        MCF_CANMB_CTRL(i) = MCF_CANMB_CTRL_CODE(0b0100);
    }

    /* configure buffers 14-15 as output buffers */
        MCF_CANMB_CTRL14 = MCF_CANMB_CTRL_CODE(0b1000);
        MCF_CANMB_CTRL15 = MCF_CANMB_CTRL_CODE(0b1000);


    /* CAN interrupt registers */
    //Enable all buffer interrupts (can be either Rx or Tx interrupt depending on buffer configuration)
    MCF_FlexCAN_IMASK = 0xFF;
    MCF_FlexCAN_IFLAG = 0xFF;
    return CO_ERROR_NO;
}
#endif


/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule)
{
    CO_CANsetConfigurationMode(CANmodule->CAN_interface_ID);
}


/******************************************************************************/
uint16_t CO_CANrxMsg_readIdent(const CO_CANrxMsg_t *rxMsg){
    return (uint16_t) (rxMsg->sid>>5);
}


/******************************************************************************/
CO_ReturnError_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        bool_t                  rtr,
        void                   *object,
        void                  (*pFunct)(void *object, const CO_CANrxMsg_t *message))
{
    CO_ReturnError_t ret = CO_ERROR_NO;

    if ((CANmodule != NULL) && (object != NULL) && (pFunct != NULL) && (index < CANmodule->rxSize)) {
        /* buffer, which will be configured */
        CO_CANrx_t *buffer = &CANmodule->rxArray[index];

        /* Configure object variables */
        buffer->object = object;
        buffer->pFunct = pFunct;

        /* CAN identifier and CAN mask, bit aligned with CAN module */
      //buffer->ident = (uint16_t) ((ident & 0x07FF)<<5);
        buffer->ident = (uint16_t)((ident & CAN_INDEX_WIDTH));
        if (rtr != 0) {
            buffer->ident |= REMOTE_FRAME_FLAG;
        }
      //buffer->mask = (uint16_t) (((mask & 0x07FF)<<5) | 0x0080);
        buffer->mask = (uint16_t)((mask & CAN_INDEX_WIDTH)/* | 0x0004*/);
        /* Set CAN hardware module filter and mask. */
      //if (CANmodule->useCANrxFilters != 0) {
      //    //filters are not used. 
      //}
    }
    else{
        ret = CO_ERROR_ILLEGAL_ARGUMENT;
    }
    return ret;
}


/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        bool_t                  rtr,
        uint8_t                 noOfBytes,
        bool_t                  syncFlag)
{
    CO_CANtx_t *buffer = NULL;

    if ((CANmodule != NULL) && (index < CANmodule->txSize)) {
        /* get specific buffer */
        buffer = &CANmodule->txArray[index];

        /* CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer */
      //buffer->ident = (uint16_t) ((ident & 0x07FF)<<5);
        buffer->ident = (uint16_t)(ident & CAN_INDEX_WIDTH);
        if (rtr != 0) {
            buffer->ident |= REMOTE_FRAME_FLAG;
        }

        buffer->DLC = noOfBytes;
        buffer->bufferFull = false;
        buffer->syncFlag = syncFlag;
    }
    return buffer;
}


/******************************************************************************/
extern QUEUE_HANDLE CANopen_interface_ID0;
extern void fnDisplayCANopen(unsigned long ulID, unsigned char *ptrData, unsigned char ucLength);

static int fnSendCAN_frame(QUEUE_HANDLE CANopen_interface_ID, unsigned char *ucData, unsigned char ucLength, unsigned short usIdent)
{
    int iRtn = 0;
    QUEUE_TRANSFER transferDetails;
    unsigned char ucMessage[12];
    uMemcpy(&ucMessage[4], ucData, ucLength);
    ucLength += 4;
    if ((usIdent & REMOTE_FRAME_FLAG) != 0) {                                       // if responding to a remote transmission request
        transferDetails |= (ucLength | CAN_TX_ACK_ON | SPECIFIED_ID | TX_REMOTE_FRAME);
        usIdent &= ~(REMOTE_FRAME_FLAG);
        fnDebugMsg("(RTR) ");
    }
    else {
        transferDetails = (ucLength | CAN_TX_ACK_ON | SPECIFIED_ID);
    }
    ucMessage[0] = 0;
    ucMessage[1] = 0;
    ucMessage[2] = (unsigned char)(usIdent >> 8);
    ucMessage[3] = (unsigned char)(usIdent);
    iRtn = (fnWrite(CANopen_interface_ID, ucMessage, transferDetails) == ucLength);
    if (iRtn != 0) {
        fnDebugMsg("CANopen Tx: ");
        fnDisplayCANopen(usIdent, &ucMessage[4], (ucLength - 4));
        fnDebugMsg("\r\n");
    }
    return iRtn;
}

CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer)
{
    CO_ReturnError_t err = CO_ERROR_NO;
  //uint16_t addr = CANmodule->CANbaseAddress;
  //uint8_t i;

    if (buffer->bufferFull != 0) {                                       // verify overflow
        if(CANmodule->firstCANtxMessage == 0){
            /* don't set error, if bootup message is still on buffers */
            CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_CAN_TX_OVERFLOW, CO_EMC_CAN_OVERRUN, /*buffer->CMSGSID*/0);
        }
        err = CO_ERROR_TX_OVERFLOW;
    }

    /* Try to find a free sending buffer */
    /* Here, we use buffer 14 & 15 for Tx */
  //i=14; //initial sending buffer index
#if 1
    if (fnSendCAN_frame(CANmodule->CAN_interface_ID, buffer->data, buffer->DLC, buffer->ident) == 0) {
        buffer->bufferFull = true;                                       // if no buffer is free, message will be sent by interrupt
        CANmodule->CANtxCount++;
    }
#else
    while(    (MCF_CANMB_CTRL(i) != MCF_CANMB_CTRL_CODE(0b1000))&&
                    (MCF_CANMB_CTRL(i) != MCF_CANMB_CTRL_CODE(0b0100))&&
                    (MCF_CANMB_CTRL(i) != MCF_CANMB_CTRL_CODE(0b1010))&&
                    i<16)//end sending buffer index+1
                        {
                            i++;
                        }

    CO_LOCK_CAN_SEND();
    /* if CAN TX buffer is free, copy message to it */
    if(i<16){
        MCF_CANMB_CTRL(i)    = 0x0000|MCF_CANMB_CTRL_CODE(0b1000); //Tx MB inactive
        MCF_CANMB_SID(i)    = buffer->ident;
        MCF_CANMB_DATA_WORD_1(i) = (uint16_t) (((buffer->data[0])<<8) | (buffer->data[1]));
        MCF_CANMB_DATA_WORD_2(i) = (uint16_t) (((buffer->data[2])<<8) | (buffer->data[3]));
        MCF_CANMB_DATA_WORD_3(i) = (uint16_t) (((buffer->data[4])<<8) | (buffer->data[5]));
        MCF_CANMB_DATA_WORD_4(i) = (uint16_t) (((buffer->data[6])<<8) | (buffer->data[7]));
        MCF_CANMB_CTRL(i)            = (uint8_t) (MCF_CANMB_CTRL_CODE(0b1000) | MCF_CANMB_CTRL_LENGTH(buffer->DLC) ); //Tx MB active
    }
    /* if no buffer is free, message will be sent by interrupt */
    else{
        buffer->bufferFull = true;
        CANmodule->CANtxCount++;
    }
#endif
    CO_UNLOCK_CAN_SEND();

    return err;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){
#if 0
    CO_LOCK_CAN_SEND();
    if(CANmodule->bufferInhibitFlag){
        MCF_CANMB_CTRL14 = MCF_CANMB_CTRL_CODE(0b1000);  //clear TXREQ
            MCF_CANMB_CTRL15 = MCF_CANMB_CTRL_CODE(0b1000);  //clear TXREQ
        CO_UNLOCK_CAN_SEND();
        CO_errorReport((CO_emergencyReport_t*)CANmodule->em, ERROR_TPDO_OUTSIDE_WINDOW, 0);
    }
    else{
        CO_UNLOCK_CAN_SEND();
    }
#endif
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
  //unsigned rxErrors, txErrors, ESTAT;
  //uint32_t err;
#if 0
    CO_emergencyReport_t* em = (CO_emergencyReport_t*)CANmodule->em;
    rxErrors = MCF_FlexCAN_RXECTR;
    txErrors = MCF_FlexCAN_TXECTR;
    ESTAT = MCF_FlexCAN_ESTAT & 0xFF00;

    if(txErrors > 0xFFFF) txErrors = 0xFFFF;
    if(rxErrors > 0xFF) rxErrors = 0xFF;

    err = (uint32_t) (txErrors << 16 | rxErrors << 8 | ESTAT>>8);

    if(CANmodule->errOld != err){
        CANmodule->errOld = err;

        if(txErrors >= 256){                                    //bus off
            CO_errorReport(em, ERROR_CAN_TX_BUS_OFF, err);
        }
        else{                                                   //not bus off
            CO_errorReset(em, ERROR_CAN_TX_BUS_OFF, err);

            if(rxErrors >= 96 || txErrors >= 96){               //bus warning
                CO_errorReport(em, ERROR_CAN_BUS_WARNING, err);
            }

            if(rxErrors >= 128){                                //RX bus passive
                CO_errorReport(em, ERROR_CAN_RX_BUS_PASSIVE, err);
            }
            else{
                CO_errorReset(em, ERROR_CAN_RX_BUS_PASSIVE, err);
            }

            if(txErrors >= 128){                                //TX bus passive
                if(!CANmodule->firstCANtxMessage){
                    CO_errorReport(em, ERROR_CAN_TX_BUS_PASSIVE, err);
                }
            }
            else{
                int16_t wasCleared;
                wasCleared =        CO_errorReset(em, ERROR_CAN_TX_BUS_PASSIVE, err);
                if(wasCleared == 1) CO_errorReset(em, ERROR_CAN_TX_OVERFLOW, err);
            }

            if(rxErrors < 96 && txErrors < 96){                 //no error
                int16_t wasCleared;
                wasCleared =        CO_errorReset(em, ERROR_CAN_BUS_WARNING, err);
                if(wasCleared == 1) CO_errorReset(em, ERROR_CAN_TX_OVERFLOW, err);
            }
        }

        if(ESTAT&MCF_FlexCAN_ESTAT_TXWARN && ESTAT&MCF_FlexCAN_ESTAT_RXWARN){//bus warning
            CO_errorReport(em, ERROR_CAN_BUS_WARNING, err);
        }
        else{
            CO_errorReset(em, ERROR_CAN_BUS_WARNING, err);
        }
    }
#endif
}

extern void fnCANopenRx(int iInstance, unsigned long ulID, unsigned char *ucInputMessage, int iLength, unsigned short usTimeStamp)
{
    CO_CANmodule_t *CANmodule = CO[iInstance]->CANmodule[0];
    uint16_t rcvMsgIdent;       /* identifier of the received message */
    uint16_t index;             /* index of received message */
    CO_CANrx_t *buffer = NULL;  /* receive message buffer from CO_CANmodule_t object. */
                                /* CAN module filters are not used, message with any standard 11-bit identifier */
                                /* has been received. Search rxArray form CANmodule for the same CAN-ID. */
    rcvMsgIdent = (uint16_t)ulID;
    buffer = &CANmodule->rxArray[0];
    for (index = CANmodule->rxSize; index > 0U; index--) {
        if (((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U) {
            /* Call specific function, which will process the message */
            if (buffer->pFunct != NULL) {
                CO_CANrxMsg_t rcvMsg;
                rcvMsg.code = 0;
                uMemcpy(rcvMsg.data, ucInputMessage, iLength);
                rcvMsg.DLC = (unsigned char)iLength;
                rcvMsg.RTR_FLAG = 0;
                rcvMsg.sid = 0;
                rcvMsg.timestamp = (unsigned char)usTimeStamp;
                rcvMsg.timestamp16 = usTimeStamp;
                buffer->pFunct(buffer->object, &rcvMsg);
            }
            break;
        }
        buffer++;
    }
}


extern void fnCANopenTxOK(int iInstance)
{
    CO_CANmodule_t *CANmodule = CO[iInstance]->CANmodule[0];
    /* First CAN message (bootup) was sent successfully */
    CANmodule->firstCANtxMessage = false;
    /* clear flag from previous message */
    CANmodule->bufferInhibitFlag = false;
    /* Are there any new messages waiting to be send */
    if (CANmodule->CANtxCount > 0U) {
        uint16_t i;             /* index of transmitting message */
        CO_CANtx_t *buffer;     /* Tx buffer */

                                /* first buffer */
        buffer = &CANmodule->txArray[0];
        /* search through whole array of pointers to transmit message buffers. */
        for (i = CANmodule->txSize; i > 0U; i--) {
            /* if message buffer is full, send it. */
            if (buffer->bufferFull) {
                buffer->bufferFull = false;
                CANmodule->CANtxCount--;

                /* Copy message to CAN buffer */
                CANmodule->bufferInhibitFlag = buffer->syncFlag;
#if 0
                MCF_CANMB_CTRL(ICODE) = 0x0000 | MCF_CANMB_CTRL_CODE(0b1000); //Tx MB inactive
                MCF_CANMB_SID(ICODE) = buffer->ident;
                MCF_CANMB_DATA_WORD_1(ICODE) = (uint16_t)(((buffer->data[0]) << 8) | (buffer->data[1]));
                MCF_CANMB_DATA_WORD_2(ICODE) = (uint16_t)(((buffer->data[2]) << 8) | (buffer->data[3]));
                MCF_CANMB_DATA_WORD_3(ICODE) = (uint16_t)(((buffer->data[4]) << 8) | (buffer->data[5]));
                MCF_CANMB_DATA_WORD_4(ICODE) = (uint16_t)(((buffer->data[6]) << 8) | (buffer->data[7]));
                MCF_CANMB_CTRL(ICODE) = (uint8_t)(MCF_CANMB_CTRL_CODE(0b1000) | MCF_CANMB_CTRL_LENGTH(buffer->DLC)); //Tx MB active
#endif
                fnSendCAN_frame(CANmodule->CAN_interface_ID, buffer->data, buffer->DLC, buffer->ident);
                break;                      /* exit for loop */
            }
            buffer++;
        }/* end of for loop */

         /* Clear counter if no more messages */
        if (i == 0U) {
            CANmodule->CANtxCount = 0U;
        }
    }
}

#if 0

/******************************************************************************/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule, uint16_t ICODE)
{

    /* receive interrupt (New CAN message is available in one of the Rx buffers) */
    if(ICODE <= 13){
        CO_CANrxMsg_t *rcvMsg;      /* pointer to received message in CAN module */
        uint16_t index;             /* index of received message */
        uint16_t rcvMsgIdent;       /* identifier of the received message */
        CO_CANrx_t *buffer = NULL;  /* receive message buffer from CO_CANmodule_t object. */
        bool_t msgMatched = false;
        rcvMsg = &MCF_CANMB_MSG(ICODE);    //structures are aligned
        rcvMsgIdent = (uint16_t) rcvMsg->sid;
        if(rcvMsg->RTR) rcvMsgIdent |= 0x0800;

            /* CAN module filters are not used, message with any standard 11-bit identifier */
            /* has been received. Search rxArray form CANmodule for the same CAN-ID. */
            buffer = &CANmodule->rxArray[0];
            for(index = CANmodule->rxSize; index > 0U; index--){
                if(((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U){
                    msgMatched = true;
                    break;
                }
                buffer++;
            }

        /* Call specific function, which will process the message */
        if(msgMatched && (buffer != NULL) && (buffer->pFunct != NULL)){
            buffer->pFunct(buffer->object, rcvMsg);
        }

    }


    /* transmit interrupt (TX buffer 14 or 15 has finished sending) */
    else if(ICODE >14 ){
        /* First CAN message (bootup) was sent successfully */
        CANmodule->firstCANtxMessage = false;
        /* clear flag from previous message */
        CANmodule->bufferInhibitFlag = false;
        /* Are there any new messages waiting to be send */
        if(CANmodule->CANtxCount > 0U){
            uint16_t i;             /* index of transmitting message */
            CO_CANtx_t *buffer;     /* Tx buffer */

            /* first buffer */
            buffer = &CANmodule->txArray[0];
            /* search through whole array of pointers to transmit message buffers. */
            for(i = CANmodule->txSize; i > 0U; i--){
                /* if message buffer is full, send it. */
                if(buffer->bufferFull){
                    buffer->bufferFull = false;
                    CANmodule->CANtxCount--;

                    /* Copy message to CAN buffer */
                    CANmodule->bufferInhibitFlag = buffer->syncFlag;
                    MCF_CANMB_CTRL(ICODE)           = 0x0000|MCF_CANMB_CTRL_CODE(0b1000); //Tx MB inactive
                    MCF_CANMB_SID(ICODE)            = buffer->ident;
                    MCF_CANMB_DATA_WORD_1(ICODE)    = (uint16_t) (((buffer->data[0])<<8) | (buffer->data[1]));
                    MCF_CANMB_DATA_WORD_2(ICODE)    = (uint16_t) (((buffer->data[2])<<8) | (buffer->data[3]));
                    MCF_CANMB_DATA_WORD_3(ICODE)    = (uint16_t) (((buffer->data[4])<<8) | (buffer->data[5]));
                    MCF_CANMB_DATA_WORD_4(ICODE)    = (uint16_t) (((buffer->data[6])<<8) | (buffer->data[7]));
                    MCF_CANMB_CTRL(ICODE)           = (uint8_t) (MCF_CANMB_CTRL_CODE(0b1000) | MCF_CANMB_CTRL_LENGTH(buffer->DLC) ); //Tx MB active
                    break;                      /* exit for loop */
                }
                buffer++;
            }/* end of for loop */

            /* Clear counter if no more messages */
            if(i == 0U){
                CANmodule->CANtxCount = 0U;
            }
        }
    }
}
#endif
#endif
