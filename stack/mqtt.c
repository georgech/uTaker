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
#define MQTT_CONTROL_PACKET_FLAG_QoS_0        0x00                       // publish quality of service level 0 (at most once delivery)
#define MQTT_CONTROL_PACKET_FLAG_QoS_1        0x02                       // publish quality of service level 1 (at least once delivery)
#define MQTT_CONTROL_PACKET_FLAG_QoS_2        0x04                       // publish quality of service level 2 (exactly once delivery)
#define MQTT_CONTROL_PACKET_FLAG_DUP          0x08                       // only used by publish control packet - duplicate delivery of a publish control packet

#define MQTT_PROTOCOL_LEVEL                   4                          // version 3.1.1

#define MQTT_CONNECT_FLAG_CLEAN_SESSION       0x02
#define MQTT_CONNECT_FLAG_WILL_FLAG           0x04
#define MQTT_CONNECT_FLAG_WILL_QOS_MASK       0x18
#define MQTT_CONNECT_FLAG_WILL_RETAIN         0x20
#define MQTT_CONNECT_FLAG_PASSWORD_FLAG       0x40
#define MQTT_CONNECT_FLAG_USER_NAME_FLAG      0x80


#define MQTT_STATE_CLOSED                     0x00
#define MQTT_STATE_OPEN_REQUESTED             0x11
#define MQTT_STATE_OPEN_SENT                  0x21
#define MQTT_STATE_CONNECTION_OPENED          0x31
#define MQTT_STATE_SUBSCRIBE                  0x41
#define MQTT_STATE_SUBSCRIBED                 0x50
#define MQTT_STATE_PUBLISH                    0x61
#define MQTT_STATE_PUBLISH_RELEASE            0x71
#define MQTT_STATE_CONNECTED_IDLE             0x80
#define MQTT_STATE_CLOSING                    0x91


static USOCKET        MQTT_TCP_socket = -1;
static unsigned char  ucMQTT_ip_address[IPV4_LENGTH] = { 0 };
static unsigned char  ucMQTT_state = MQTT_STATE_CLOSED;
static unsigned short usPacketIdentifier = 1;

static int  fnMQTTListener(signed char cSocket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static int  fnSetNextMQTT_state(unsigned char ucNextState);
static void fnMQTT_error(unsigned char ucError);

static unsigned char cucProtocolNameMQTT[] = { 0x00, 0x04,               // length
                                               'M', 'Q', 'T', 'T'        // name
};

#define PUBLISH_QoS_LEVEL          MQTT_CONTROL_PACKET_FLAG_QoS_2        // option

#define MQTT_CLIENT_TIMEOUT        (DELAY_LIMIT)(60 * SEC)
#define E_MQTT_TIMEOUT             1


static unsigned short (*fnUserCallback)(unsigned char, unsigned char *, unsigned char *) = 0;
static unsigned short fnRegenerate(void);
static int  fnHandleData(unsigned char *ptrData, unsigned short usDataLength);


static unsigned char  ucUnacked = 0;
static unsigned char  ucQueueFlags = 0;
#define MQTT_QUEUE_CLOSE      0x01
#define MQTT_QUEUE_PUBLISH    0x02


// Not yet used
//
extern void fnMQTT(TTASKTABLE *ptrTaskTable)
{    
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) {    // check input queue
        if (ucInputMessage[MSG_SOURCE_TASK] == TIMER_EVENT) {
            if (E_MQTT_TIMEOUT == ucInputMessage[MSG_TIMER_EVENT]) {
              //fnMQTT_error(ERROR_MQTT_TIMEOUT);
            }
        }
    }
}

// The user calls this to initiate a connection to the MQTT server/broker
//
extern int fnConnectMQTT(unsigned char *ucIP, unsigned short(*fnCallback)(unsigned char, unsigned char *, unsigned char *))
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

// The user calls this to disconnect from the MQTT server/broker
//
extern int fnDisconnectMQTT(void)
{
    if ((MQTT_TCP_socket < 0) || (ucMQTT_state < MQTT_STATE_CONNECTION_OPENED)) { // we have no socket - or called when not connected
        return ERROR_MQTT_NOT_READY;
    }
    ucQueueFlags = MQTT_QUEUE_CLOSE;
    if ((ucMQTT_state & 0x01) == 0) {                                    // no busy with other commands (otherwise it will be exected at next possible opportunity)
        fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);
    }
    return 0;                                                            // OK    
}

extern int fnPublishMQTT(void)
{
    if ((MQTT_TCP_socket < 0) || (ucMQTT_state < MQTT_STATE_CONNECTION_OPENED)) { // we have no socket - or called when not connected
        return ERROR_MQTT_NOT_READY;
    }
    ucQueueFlags |= MQTT_QUEUE_PUBLISH;
    if ((ucMQTT_state & 0x01) == 0) {                                    // no busy with other commands (otherwise it will be exected at next possible opportunity)
        fnSetNextMQTT_state(MQTT_STATE_PUBLISH);
    }
    return 0;                                                            // OK    
}

static int fnSetNextMQTT_state(unsigned char ucNextState)
{
    switch (ucMQTT_state = ucNextState) {
      case MQTT_STATE_OPEN_REQUESTED:
          fnTCP_close(MQTT_TCP_socket);                                  // release existing connection
          if (fnTCP_Connect(MQTT_TCP_socket, ucMQTT_ip_address, MQTT_PORT, 0, 0) >= 0) { // start connection with MQTT broker
              ucMQTT_state = MQTT_STATE_OPEN_SENT;
              ucUnacked = 0;
              return 1;                                                  // connection request sent
          }
          break;
      case MQTT_STATE_CLOSED:                                            // TCP connection has been closed
          ucUnacked = 0;
          ucQueueFlags = 0;
          break;
      case MQTT_STATE_CONNECTION_OPENED:
      case MQTT_STATE_SUBSCRIBE:
      case MQTT_STATE_PUBLISH:
      case MQTT_STATE_PUBLISH_RELEASE:
          return (fnRegenerate() > 0);                                   // send or repeat transmission
      case MQTT_STATE_CONNECTED_IDLE:
          if ((ucQueueFlags & MQTT_QUEUE_CLOSE) != 0) {                  // if a close has been queued
              ucQueueFlags = 0;
              if (fnTCP_close(MQTT_TCP_socket) != 0) {                   // command close of data connection
                  return APP_REQUEST_CLOSE;
              }
          }
          break;
      default:
          break;
    }
    return (0);
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
        fnMQTT_error(ERROR_MQTT_ARP_FAIL);                               // inform client of failure - couldn't resolve the address..
        break;

    case TCP_EVENT_CONNECTED:                                            // the broker has accepted the TCP connection request
        if (ucMQTT_state == MQTT_STATE_OPEN_SENT) {
            return (fnSetNextMQTT_state(MQTT_STATE_CONNECTION_OPENED));
        }
        break;

    case TCP_EVENT_ACK:                                                  // last TCP transmission has been acknowledged
        ucUnacked = 0;                                     
        break;

    case TCP_EVENT_CLOSE:                                                // broker is requesting a close
        fnSetNextMQTT_state(MQTT_STATE_CLOSING);
        break;

    case TCP_EVENT_ABORT:
        fnMQTT_error(MQTT_HOST_CLOSED);
        break;
    case TCP_EVENT_CLOSED:
        fnSetNextMQTT_state(MQTT_STATE_CLOSED);
        fnUserCallback(MQTT_CONNECTION_CLOSED, 0, 0);
        break;

    case TCP_EVENT_REGENERATE:                                           // we must repeat the previous transmission
        return (fnRegenerate() > 0);

    case TCP_EVENT_DATA:                                                 // we have new receive data
        if (ucUnacked != 0) {
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

unsigned char *fnMQTTUserString(unsigned char *ptrMQTT_packet, unsigned char ucEvent, unsigned char *ptrReference)
{
    unsigned char *ptrStringLength = ptrMQTT_packet;
    unsigned short usStringLength;
    ptrMQTT_packet += 2;                                                 // leave space for the string length
    usStringLength = fnUserCallback(ucEvent, ptrReference, ptrMQTT_packet); // request user to add the string
    *ptrStringLength++ = (unsigned char)(usStringLength >> 8);           // insert the length before the string content
    *ptrStringLength++ = (unsigned char)(usStringLength);
    return (ptrMQTT_packet + usStringLength);
}

unsigned char *fnInsertPacketIdentfier(unsigned char *ptrMQTT_packet, unsigned char ucControlPacketType)
{
    *ptrMQTT_packet++ = (unsigned char)(usPacketIdentifier >> 8);
    *ptrMQTT_packet++ = (unsigned char)(usPacketIdentifier);
    return ptrMQTT_packet;
}

static void fnIncrementtPacketIdentfier(unsigned char ucControlPacketType)
{
    if (++usPacketIdentifier == 0) {                                     // avoid invalid value of zero
        usPacketIdentifier = 1;
    }
}


static unsigned short fnRegenerate(void)
{
    unsigned char ucMQTTData[MIN_TCP_HLEN + MQTT_MESSAGE_LEN];
    unsigned short usDataLen = 5;                                        // general length of strings
    unsigned char *ptrMQTT_packet = (unsigned char *)&ucMQTTData[MIN_TCP_HLEN];
    unsigned char *ptrRemainingLength;

    switch (ucMQTT_state) {                                              // resent last packet
    case MQTT_STATE_CONNECTION_OPENED:                                   // client to broker after establishing the TCP connection
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
        ptrMQTT_packet = fnMQTTUserString(ptrMQTT_packet, MQTT_CLIENT_IDENTIFIER, 0); // callback to get the client idetifier (should normally contain only 0..9, a..z, A..Z characters)
        break;

    case MQTT_STATE_SUBSCRIBE:
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_SUBSCRIBE | MQTT_CONTROL_PACKET_FLAG_QoS_1); // the QoS is always 1 for a subscribe
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, (MQTT_CONTROL_PACKET_TYPE_SUBSCRIBE >> 4));
        // Topic filter
        //
        ptrMQTT_packet = fnMQTTUserString(ptrMQTT_packet, MQTT_PUBLISH_TOPIC_FILTER, 0); // request the topic filter from the user
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_FLAG_QoS_1 >> 1);
        break;

    case MQTT_STATE_PUBLISH:                                             // client to broker or broker to client - transport an application message
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_PUBLISH | PUBLISH_QoS_LEVEL);
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        // Topic name
        //
        ptrMQTT_packet = fnMQTTUserString(ptrMQTT_packet, MQTT_PUBLISH_TOPIC, 0); // request the topic from the user

        // Packet identifier - only present when QoS 1 or 2 is used! Must be non-zero - the respone needs to match!
        //
    #if PUBLISH_QoS_LEVEL > MQTT_CONTROL_PACKET_FLAG_QoS_0
        ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, (MQTT_CONTROL_PACKET_TYPE_PUBLISH >> 4));
    #endif

        // Application data
        //
        ptrMQTT_packet += fnUserCallback(MQTT_PUBLISH_DATA, 0, ptrMQTT_packet); // request user to insert the message content - can be any format and length, including zero length
        break;
    #if PUBLISH_QoS_LEVEL == MQTT_CONTROL_PACKET_FLAG_QoS_2
    case MQTT_STATE_PUBLISH_RELEASE:
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_PUBREL | MQTT_CONTROL_PACKET_FLAG_QoS_1);
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, (MQTT_CONTROL_PACKET_TYPE_PUBLISH >> 4));
        break;
    #endif
    default:
        return 0;
    }
    usDataLen = fnAddMQTT_remaining_length(ptrMQTT_packet, (unsigned char *)&ucMQTTData[MIN_TCP_HLEN], MQTT_MESSAGE_LEN, ptrRemainingLength);
    return (fnSendTCP(MQTT_TCP_socket, ucMQTTData, usDataLen, TCP_FLAG_PUSH) > 0); // send data
}

// Handle receptions from the broker
//
static int fnHandleData(unsigned char *ptrData, unsigned short usDataLength)
{
    switch (ucMQTT_state) {
    case MQTT_STATE_CONNECTION_OPENED:
        if ((*ptrData & MQTT_CONTROL_PACKET_TYPE_MASK) == MQTT_CONTROL_PACKET_TYPE_CONNACK) { // the broker is accepting the MQTT connection
            fnUserCallback(MQTT_CONNACK_RECEIVED, 0, 0);                 // inform the user that the broker has accepted
            return (fnSetNextMQTT_state(MQTT_STATE_SUBSCRIBE));          // now subscribe
        }
        else {

        }
        break;
    case MQTT_STATE_SUBSCRIBE:
        if ((*ptrData & MQTT_CONTROL_PACKET_TYPE_MASK) == MQTT_CONTROL_PACKET_TYPE_SUBACK) { // the broker is accepting the subscription
            CHAR *message[10];
            fnIncrementtPacketIdentfier((MQTT_CONTROL_PACKET_TYPE_SUBSCRIBE >> 4));
            fnSetNextMQTT_state(MQTT_STATE_SUBSCRIBED);
            if (fnUserCallback(MQTT_SUBACK_RECEIVED, 0, (unsigned char *)message) != 0) { // inform the user that the broker has accepted
                return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH));        // user wants to immediately publish a message
            }
        }
        else {

        }
        break;
    case MQTT_STATE_PUBLISH:
        if ((*ptrData & MQTT_CONTROL_PACKET_TYPE_MASK) == MQTT_CONTROL_PACKET_TYPE_PUBREC) { // the broker is accepting to publish
            if (usDataLength == 4) {
                if (((unsigned char)(usPacketIdentifier >> 8) == *(ptrData + 2)) && (((unsigned char)(usPacketIdentifier) == *(ptrData + 3)))) {
    #if PUBLISH_QoS_LEVEL == MQTT_CONTROL_PACKET_FLAG_QoS_2
                    return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH_RELEASE)); // send the third packet of QoS 2 protocol exchange
    #else
                    CHAR *message[10];
                    fnIncrementtPacketIdentfier((MQTT_CONTROL_PACKET_TYPE_PUBLISH >> 4));
                    if (fnUserCallback(MQTT_PUBLISH_RECEIVED, 0, (unsigned char *)message) != 0) { // inform the user that the broker has accepted to publish
                        return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH));// user wants to immediately publish a new message
                    }
    #endif
                }
            }
        }
        else {

        }
        break;
#if PUBLISH_QoS_LEVEL == MQTT_CONTROL_PACKET_FLAG_QoS_2
    case MQTT_STATE_PUBLISH_RELEASE:
        if ((*ptrData & MQTT_CONTROL_PACKET_TYPE_MASK) == MQTT_CONTROL_PACKET_TYPE_PUBCOMP) { // the broker is accepting to publish
            if (usDataLength == 4) {
                if (((unsigned char)(usPacketIdentifier >> 8) == *(ptrData + 2)) && (((unsigned char)(usPacketIdentifier) == *(ptrData + 3)))) {
                    CHAR *message[10];
                    fnIncrementtPacketIdentfier((MQTT_CONTROL_PACKET_TYPE_PUBLISH >> 4));
                    if (fnUserCallback(MQTT_PUBLISH_RECEIVED, 0, (unsigned char *)message) != 0) { // inform the user that the broker has accepted to publish
                        return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH));// user wants to immediately publish a new message
                    }
                    fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);
                }
            }
        }
        else {

        }
        break;
#endif  
    }
    return 0;
}


static void fnMQTT_error(unsigned char ucError)
{
    fnSetNextMQTT_state(MQTT_STATE_CLOSED);
    fnUserCallback(ucError, 0, 0);
}
#endif
