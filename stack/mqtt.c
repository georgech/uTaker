/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      mqtt.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    
*/        

#include "config.h"


#if defined USE_MQTT_CLIENT || defined USE_MQTT_SERVER


#define OWN_TASK     TASK_MQTT


typedef struct stMQTT_CONTROL_PACKET_HEADER {
    unsigned char ucMQTT_control_packet_type_flags;
    unsigned char ucRemainingLength[1];                                  // 1.. 4 bytes
} MQTT_CONTROL_PACKET_HEADER;

#define MQTT_CONTROL_PACKET_TYPE_Reserved_0   (0 << 4)                   // forbidden
#define MQTT_CONTROL_PACKET_TYPE_CONNECT      (1 << 4)                   // client to server connection request
#define MQTT_CONTROL_PACKET_TYPE_CONNACK      (2 << 4)                   // server to client connect acknowledgment
#define MQTT_CONTROL_PACKET_TYPE_PUBLISH      (3 << 4)                   // publish message (both client and server)
#define MQTT_CONTROL_PACKET_TYPE_PUBACK       (4 << 4)                   // publish acknowledgement (both client and server)
#define MQTT_CONTROL_PACKET_TYPE_PUBREC       (5 << 4)                   // publish received (both client and server) - assured delivery part 1
#define MQTT_CONTROL_PACKET_TYPE_PUBREL       (6 << 4)                   // publish releae (both client and server) - assured delivery part 2
#define MQTT_CONTROL_PACKET_TYPE_PUBCOMP      (7 << 4)                   // publish complete (both client and server) - assured delivery part 3
#define MQTT_CONTROL_PACKET_TYPE_SUBSCRIBE    (8 << 4)                   // client to server subscribe request
#define MQTT_CONTROL_PACKET_TYPE_SUBACK       (9 << 4)                   // server to client subscribe ack
#define MQTT_CONTROL_PACKET_TYPE_UNSUBSCRIBE  (10 << 4)                  // client to server unsubscribe request
#define MQTT_CONTROL_PACKET_TYPE_UNSUBACK     (11 << 4)                  // server to client unsubscribe ack
#define MQTT_CONTROL_PACKET_TYPE_PINGREQ      (12 << 4)                  // client to server ping request
#define MQTT_CONTROL_PACKET_TYPE_PINGACK      (13 << 4)                  // server to client ping ack
#define MQTT_CONTROL_PACKET_TYPE_DISCONNECT   (14 << 4)                  // client to server - client is disconnecting
#define MQTT_CONTROL_PACKET_TYPE_Reserved_15  (15 << 4)                  // forbidden

#define MQTT_CONTROL_PACKET_TYPE_MASK         0xf0

#define MQTT_CONTROL_PACKET_FLAG_RETAIN       0x01                       // only used by publish control packet - publish retain flag
#define MQTT_CONTROL_PACKET_FLAG_QoS          0x06                       // only used by publish control packet - publish quality of service mask
#define MQTT_CONTROL_PACKET_FLAG_DUP          0x08                       // only used by publish control packet - duplicate delivery of a publish control packet

#define MQTT_PROTOCOL_LEVEL                   4                          // version 3.1.1

#define MQTT_CONNECT_FLAG_CLEAN_SESSION       0x02
#define MQTT_CONNECT_FLAG_WILL_FLAG           0x04
#define MQTT_CONNECT_FLAG_WILL_QOS_MASK       0x18
#define MQTT_CONNECT_FLAG_WILL_RETAIN         0x20
#define MQTT_CONNECT_FLAG_PASSWORD_FLAG       0x40
#define MQTT_CONNECT_FLAG_USER_NAME_FLAG      0x80


#define MQTT_STATE_CLOSED            0
#define MQTT_STATE_OPEN_REQUESTED    1
#define MQTT_STATE_OPEN_SENT         2
#define MQTT_STATE_CONNECTION_OPENED 3
#define MQTT_STATE_SUBSCRIBE         4
#define MQTT_STATE_SUBSCRIBED        5
#define MQTT_STATE_PUBLISH           6


static USOCKET        MQTT_TCP_socket = -1;
static unsigned char  ucMQTT_ip_address[IPV4_LENGTH] = { 0 };
static unsigned char  ucMQTT_state = MQTT_STATE_CLOSED;

static int  fnMQTTListener(signed char cSocket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static int  fnSetNextMQTT_state(unsigned char ucNextState);
static void fnMQTT_error(unsigned char ucError);

static unsigned char cucProtocolNameMQTT[] = { 0x00, 0x04,                // length
                                               'M', 'Q', 'T', 'T'         // name
};


#define MQTT_CLIENT_TIMEOUT        (DELAY_LIMIT)(20 * SEC)
#define E_MQTT_TIMEOUT             1
#define E_MQTT_POLL_TIME           2


static CHAR *(*fnUserCallback)(unsigned char, unsigned char*) = 0;
static unsigned short fnRegenerate(void);
static int  fnHandleData(unsigned char *ptrData, unsigned short usDataLength);


static unsigned char  ucUnacked = 0;
static unsigned short usMsgCnt;


extern void fnMQTT(TTASKTABLE *ptrTaskTable)
{    
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) {    // check input queue
        if (ucInputMessage[MSG_SOURCE_TASK] == TIMER_EVENT) {
            if (E_MQTT_TIMEOUT == ucInputMessage[MSG_TIMER_EVENT]) {
              //fnMQTT_error(ERROR_POP3_TIMEOUT);
            }
            else {                                                   // assume E_POP_POLL_TIME
                //  if (uMemcmp(ucMQTT_ip_address, cucNullMACIP, IPV4_LENGTH) != 0) {
                //    fnConnectPOP3(ucPOP3_ip_address);                // check our mailbox to see whether we have post
                //}
            }
        }
    }
}

// The user calls this to initiate a connection to the MQTT server/broker
//
extern int fnConnectMQTT(unsigned char *ucIP, CHAR *(*fnCallback)(unsigned char, unsigned char *))
{
    if (MQTT_TCP_socket < 0) {                                           // we have no socket - or called before initialisation complete    
        if ((MQTT_TCP_socket = fnGetTCP_Socket(TOS_MINIMISE_DELAY, TCP_DEFAULT_TIMEOUT, fnMQTTListener)) < 0) {
            return ERROR_MQTT_NOT_READY;
        }
    }

    if (ucMQTT_state != MQTT_STATE_CLOSED) {
        return ERROR_MQTT_IN_USE;                                        // called while already active
    }

    fnUserCallback = fnCallback;
    uMemcpy(ucMQTT_ip_address, ucIP, IPV4_LENGTH);                       // save the address of the MQTT server/broker we want to connect to
    fnSetNextMQTT_state(MQTT_STATE_OPEN_REQUESTED);
    return 0;                                                            // OK    
}


static int fnSetNextMQTT_state(unsigned char ucNextState)
{
    unsigned short ucSent = 0;

    switch (ucMQTT_state = ucNextState) {
      case MQTT_STATE_OPEN_REQUESTED:
          fnTCP_close(MQTT_TCP_socket);                                   // release existing connection
          if (fnTCP_Connect(MQTT_TCP_socket, ucMQTT_ip_address, MQTT_PORT, 0, 0) >= 0) {
              ucMQTT_state = MQTT_STATE_OPEN_SENT;
              ucUnacked = 0;
              return 1;
          }
          return 0;

      case MQTT_STATE_CLOSED:
          fnTCP_close(MQTT_TCP_socket);
          uTaskerStopTimer(OWN_TASK);
          ucUnacked = 0;
          fnUserCallback(MQTT_CONNECTION_CLOSED, 0);
          return APP_REQUEST_CLOSE;

      case MQTT_STATE_CONNECTION_OPENED:
      case MQTT_STATE_SUBSCRIBE:
      case MQTT_STATE_PUBLISH:
          ucSent = fnRegenerate();                                       // send user name / password etc.
          break;
      default:
          break;
    }
  //uTaskerMonoTimer(OWN_TASK, MQTT_CLIENT_TIMEOUT, E_POP_TIMEOUT);      // monitor connection
    return (ucSent > 0);
}

// local listener to TCP MQTT port
//
static int fnMQTTListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    if (Socket != MQTT_TCP_socket) {
        return APP_REJECT;                                               // ignore if not our socket 
    }

    switch (ucEvent) {
    case TCP_EVENT_ARP_RESOLUTION_FAILED:
        fnMQTT_error(ERROR_POP3_ARP_FAIL);                               // inform client of failure - couldn't resolve the address..
        break;

    case TCP_EVENT_CONNECTED:                                            // tehh broker has acepted the TCP connection request
        if (ucMQTT_state == MQTT_STATE_OPEN_SENT) {
            return (fnSetNextMQTT_state(MQTT_STATE_CONNECTION_OPENED));
        }
        break;

    case TCP_EVENT_ACK:
        ucUnacked = 0;
        break;

    case TCP_EVENT_ABORT:
        if (ucMQTT_state > MQTT_STATE_CLOSED) {
            fnMQTT_error(ERROR_POP3_HOST_CLOSED);
            return APP_REQUEST_CLOSE;
        }
        // Fall through intentional
        //
    case TCP_EVENT_CLOSED:
    case TCP_EVENT_CLOSE:
        return (fnSetNextMQTT_state(MQTT_STATE_CLOSED));

    case TCP_EVENT_REGENERATE:                                           // we must repeat
        return (fnRegenerate() > 0);

    case TCP_EVENT_DATA:                                                 // we have new receive data
        if (ucUnacked) {
            return -1;                                                   // ignore if we have unacked data
        }
        return (fnHandleData(ucIp_Data, usPortLen));                     // interpret the data

    case TCP_EVENT_CONREQ:                                               // we do not accept connection requests
    default:
        return -1;
    }

    return APP_ACCEPT;
}


static unsigned short fnAddMQTT_remaining_length(unsigned char *ptrEnd, unsigned char *ptrStart, int iMaxLength, unsigned char *ptrRemainingLength)
{
    unsigned short usDataLength = (ptrEnd - ptrStart);
    unsigned long ulRemainingLength = ((ptrEnd - ptrRemainingLength) - 1);
    if (ulRemainingLength <= 127) {
        *ptrRemainingLength = (unsigned char)ulRemainingLength;
    }
    else {
        _EXCEPTION("Implement larget length insertions");
    }
    return usDataLength;
}


#define MQTT_MESSAGE_LEN       128

static unsigned short fnRegenerate(void)
{
    unsigned char ucMQTTData[MIN_TCP_HLEN + MQTT_MESSAGE_LEN];
    unsigned short usDataLen = 5;                                        // general length of strings
    unsigned char *ptrMQTT_packet = (unsigned char *)&ucMQTTData[MIN_TCP_HLEN];
    CHAR *ptrUserInfo;
    unsigned char *ptrRemainingLength;
    unsigned char *ptrStringLength;
    unsigned short usStringLength;

    switch (ucMQTT_state) {                                              // resent last packet
    case MQTT_STATE_CONNECTION_OPENED:
        // The TCP connection has been established so we now request an MQTT connection
        //
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_CONNECT | 0);      // flags are not used by the connection connection request
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        uMemcpy(ptrMQTT_packet, cucProtocolNameMQTT, sizeof(cucProtocolNameMQTT)); // add the variable header, beginning with the fixed protocol name
        ptrMQTT_packet += sizeof(cucProtocolNameMQTT);
        *ptrMQTT_packet++ = MQTT_PROTOCOL_LEVEL;
        *ptrMQTT_packet++ = MQTT_CONNECT_FLAG_CLEAN_SESSION;
        *ptrMQTT_packet++ = 0;                                           // no keep-alive time
        *ptrMQTT_packet++ = 0;
        // Payload
        //
        ptrUserInfo = fnUserCallback(MQTT_CLIENT_IDENTIFIER, 0);         // callback to get the client idetifier (should normally contain only 0..9, a..z, A..Z characters)
        ptrStringLength = ptrMQTT_packet;
        ptrMQTT_packet += 2;
        ptrMQTT_packet = (unsigned char *)uStrcpy((CHAR *)ptrMQTT_packet, ptrUserInfo);
        usStringLength = (ptrMQTT_packet - (ptrStringLength + 2));
        *ptrStringLength++ = (unsigned char)(usStringLength >> 8);
        *ptrStringLength++ = (unsigned char)(usStringLength);
        usDataLen = fnAddMQTT_remaining_length(ptrMQTT_packet, (unsigned char *)&ucMQTTData[MIN_TCP_HLEN], MQTT_MESSAGE_LEN, ptrRemainingLength);
        break;

    case MQTT_STATE_SUBSCRIBE:
        *ptrMQTT_packet++ = 0x82;     // temp cheat
        *ptrMQTT_packet++ = 0x0d;
        *ptrMQTT_packet++ = 0x00;
        *ptrMQTT_packet++ = 0x01;
        *ptrMQTT_packet++ = 0x00;
        *ptrMQTT_packet++ = 0x08;
        *ptrMQTT_packet++ = 0x73;
        *ptrMQTT_packet++ = 0x75;
        *ptrMQTT_packet++ = 0x62;
        *ptrMQTT_packet++ = 0x74;
        *ptrMQTT_packet++ = 0x6f;
        *ptrMQTT_packet++ = 0x70;
        *ptrMQTT_packet++ = 0x69;
        *ptrMQTT_packet++ = 0x63;
        *ptrMQTT_packet++ = 0x01;
        usDataLen = 15;
        break;

    case MQTT_STATE_PUBLISH:
        *ptrMQTT_packet++ = 0x34;     // temp cheat
        *ptrMQTT_packet++ = 0x11;
        *ptrMQTT_packet++ = 0x00;
        *ptrMQTT_packet++ = 0x09;
        *ptrMQTT_packet++ = 0x48;
        *ptrMQTT_packet++ = 0x53;
        *ptrMQTT_packet++ = 0x4c;
        *ptrMQTT_packet++ = 0x55;
        *ptrMQTT_packet++ = 0x2c;
        *ptrMQTT_packet++ = 0x74;
        *ptrMQTT_packet++ = 0x65;
        *ptrMQTT_packet++ = 0x73;
        *ptrMQTT_packet++ = 0x74;
        *ptrMQTT_packet++ = 0x00;
        *ptrMQTT_packet++ = 0x02;
        *ptrMQTT_packet++ = 0x61;
        *ptrMQTT_packet++ = 0x62;
        *ptrMQTT_packet++ = 0x63;
        *ptrMQTT_packet++ = 0x64;
        usDataLen = 19;
        break;
    default:
        return 0;
    }

    return (fnSendTCP(MQTT_TCP_socket, ucMQTTData, usDataLen, TCP_FLAG_PUSH) > 0); // send data
}



static int fnHandleData(unsigned char *ptrData, unsigned short usDataLength)
{
    switch (ucMQTT_state) {
    case MQTT_STATE_CONNECTION_OPENED:
        if ((*ptrData & MQTT_CONTROL_PACKET_TYPE_MASK) == MQTT_CONTROL_PACKET_TYPE_CONNACK) { // the broker is accepting the MQTT connection
            fnUserCallback(MQTT_CONNACK_RECEIVED, 0);                    // inform the user that the broker has accepted
            return (fnSetNextMQTT_state(MQTT_STATE_SUBSCRIBE));          // now subscribe
        }
        else {

        }
        break;
    case MQTT_STATE_SUBSCRIBE:
        if ((*ptrData & MQTT_CONTROL_PACKET_TYPE_MASK) == MQTT_CONTROL_PACKET_TYPE_SUBACK) { // the broker is acceping the subscription
            CHAR *message;
            fnSetNextMQTT_state(MQTT_STATE_SUBSCRIBED);
            message = fnUserCallback(MQTT_SUBACK_RECEIVED, 0);           // inform the user that the broker has accepted
            if (message != 0) {                                          // user wants to immediately publish a message
                return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH));
            }
        }
        else {

        }
        break;
    case MQTT_STATE_PUBLISH:
        if ((*ptrData & MQTT_CONTROL_PACKET_TYPE_MASK) == MQTT_CONTROL_PACKET_TYPE_PUBREC) { // the broker is acceping to publish
            CHAR *message;
            message = fnUserCallback(MQTT_PUBLISH_RECEIVED, 0);          // inform the user that the broker has accepted to publish
            if (message != 0) {                                          // user wants to immediately publish a new message
                return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH));
            }
        }
        else {

        }
    }
    return 0;
}


static void fnMQTT_error(unsigned char ucError)
{
    fnSetNextMQTT_state(MQTT_STATE_CLOSED);
    fnUserCallback(ucError, 0);
}

#endif

