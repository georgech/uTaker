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
    Single MQTT client connection implemented using a simple socket (one outstanding transmission at a time)

*/        

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined USE_MQTT_CLIENT || defined USE_MQTT_SERVER

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK     TASK_MQTT

#define MQTT_MESSAGE_LEN       1400                                      // largest transmission supported

#define MQTT_MAX_SUBSCRIPTIONS 8
#define MQTT_MAX_TOPIC_LENGTH  16

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

#define MQTT_CONNECT_FLAG_CLEAN_SESSION       0x02                       // flag to request that a new session clears and previous session states
#define MQTT_CONNECT_FLAG_WILL_FLAG           0x04                       // flag to signal that the "will QoS" and "will retain" fields will be used by the server
#define MQTT_CONNECT_FLAG_WILL_QOS_MASK       0x18                       // the QoS level to be used when publishing the "will message" (must be set to 0 if the "will flag" is not set)
#define MQTT_CONNECT_FLAG_WILL_RETAIN         0x20                       // flag to signal that the server must publish the "will message" as a retained message (not allowed if the "will flag" is not set)
#define MQTT_CONNECT_FLAG_PASSWORD_FLAG       0x40                       // flag to indicate that a password must be present in the payload (not allowed if the user name flag is not set)
#define MQTT_CONNECT_FLAG_USER_NAME_FLAG      0x80                       // flag to indicate that a user name must be present in the payload

#define MQTT_STATE_CLOSED                     0x00                       // not connnected
#define MQTT_STATE_OPEN_REQUESTED             0x11                       // intending to connect
#define MQTT_STATE_OPEN_SENT                  0x21                       // in the process of connecting to the broker's tcp socket
#define MQTT_STATE_CONNECTION_OPENED          0x31                       // TCP connection established with the broker
#define MQTT_STATE_SUBSCRIBE                  0x41                       // in the process of subscribing
#define MQTT_STATE_UNSUBSCRIBE                0x51                       // in process of unsubscribing
#define MQTT_STATE_PUBLISH                    0x61                       // in the process of publishing
#define MQTT_STATE_PUBLISH_RELEASE            0x71                       // in the pubish handshake state
#define MQTT_STATE_CONNECTED_IDLE             0x80                       // connected and idle
#define MQTT_STATE_SENDING_KEEPALIVE          0x91                       // in the process of sending a ping to the broker
#define MQTT_STATE_PUBLISH_ACK                0xa1                       // in the process of sending a publish ack message
#define MQTT_STATE_PUBLISH_RECEIVED           0xb1                       // in the process of sending a publish received message
#define MQTT_STATE_PUBLISH_COMPLETE           0xc1                       // in the process of sending a publish complete message
#define MQTT_STATE_CLOSING                    0xd1                       // in the process of closing the connection with the broker

#define PUBLISH_QoS_LEVEL              MQTT_CONTROL_PACKET_FLAG_QoS_2    // option - always publish using QoS2

#define MQTT_SUBSCRIPTION_QoS_0               0x00
#define MQTT_SUBSCRIPTION_QoS_1               0x01
#define MQTT_SUBSCRIPTION_QoS_2               0x02

#define MQTT_KEEPALIVE_TIME            (DELAY_LIMIT)(60 * SEC)
#define MQTT_PING_TIME                 (DELAY_LIMIT)(15 * SEC)
#define MQTT_KEEPALIVE_TIME_SECONDS    300

#define T_MQTT_KEEPALIVE_TIMEOUT       1
#define T_MQTT_BROKER_DEAD             2

#define MQTT_QUEUE_CLOSE            0x01                                 // we are waiting to close
#define MQTT_QUEUE_SUBSCRIBE        0x02                                 // we are waiting to subscribe
#define MQTT_QUEUE_UNSUBSCRIBE      0x04                                 // we are waiting to unsubscribe
#define MQTT_QUEUE_PUBLISH          0x08                                 // we are waiting to publish
#define MQTT_QUEUE_PUBLISH_ACK      0x10                                 // we are waiting to respond with publish ack
#define MQTT_QUEUE_PUBLISH_RECEIVED 0x20                                 // we are waiting to respond with publish received
#define MQTT_QUEUE_REGEN            0x40                                 // we are waiting to continue with next state's transmission

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stMQTT_CONTROL_PACKET_HEADER {
    unsigned char ucMQTT_control_packet_type_flags;
    unsigned char ucRemainingLength[1];                                  // 1.. 4 bytes
} MQTT_CONTROL_PACKET_HEADER;

typedef struct stMQTT_SUBSCRIPTION_ENTRY {
    unsigned char ucSubscriptionReference;
    unsigned char ucSubscriptionQoS;                                     // 0, 1 or 2
    CHAR cSubscriptionTopic[MQTT_MAX_TOPIC_LENGTH + 1];                  // subscription topic string
} MQTT__SUBSCRIPTION_ENTRY;

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int  fnMQTTListener(signed char cSocket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static int  fnSetNextMQTT_state(unsigned char ucNextState);
static void fnMQTT_error(unsigned char ucError);
static unsigned short fnRegenerate(void);
static void fnMQTT_ping(void);
static int fnHandleData(unsigned char *ptrData, unsigned short usDataLength);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static unsigned char cucProtocolNameMQTT[] = { 0x00, 0x04,               // length
                                               'M', 'Q', 'T', 'T'        // name
};
#if 0 // temp
static unsigned char test[] = {0x34, 0x38, 0x00, 0x23, 0x2f, 0x61, 0x76, 0x6f, 0x6c, 0x61, 0x6e, 0x61, 0x2f, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2f, 0x41,
0x56, 0x4f, 0x4c, 0x41, 0x4e, 0x41, 0x37, 0x30, 0x62, 0x33, 0x64, 0x35, 0x34, 0x32, 0x38, 0x30, 0x30, 0x37, 0x00, 0x02, 0x7b, 0x22, 0x68, 0x65, 0x6c,
0x6c, 0x6f, 0x22, 0x3a, 0x22, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x22, 0x7d };
static unsigned char test2[] = { 0x62, 0x02, 0x00, 0x02 };
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static USOCKET        MQTT_TCP_socket = -1;
static unsigned char  ucMQTT_ip_address[IPV4_LENGTH] = { 0 };
static unsigned char  ucMQTT_state = MQTT_STATE_CLOSED;
static unsigned short usPacketIdentifier = 1;                            // the packet identifier that is sent - unique per publish message
static unsigned short usMessageIdentifier = 0;                           // last reception message identifier
static unsigned short (*fnUserCallback)(unsigned char, unsigned char *, unsigned long, unsigned char) = 0;
static unsigned char  ucUnacked = 0;
static unsigned char  ucQueueFlags = 0;
static unsigned char  ucSubscriptionInProgress = 0;
static unsigned char  ucPublishInProgress = 0;
static unsigned char  ucPublishQoS = 0;
static MQTT__SUBSCRIPTION_ENTRY subscriptions[MQTT_MAX_SUBSCRIPTIONS] = {{0}};


// MQTT task
//
extern void fnMQTT(TTASKTABLE *ptrTaskTable)
{    
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) {    // check input queue
        if (ucInputMessage[MSG_SOURCE_TASK] == TIMER_EVENT) {
            switch (ucInputMessage[MSG_TIMER_EVENT]) {
            case T_MQTT_KEEPALIVE_TIMEOUT:
                fnMQTT_ping();                                           // send a ping to signal that we are still there
                break;
            case T_MQTT_BROKER_DEAD:                                     // the broker hasn't replied to a ping within a reasonable time
                if (ucUnacked != 0) {                                    // if the ping is being repeated don't disturb TCP and allow it to close for us if it fails
                    uTaskerMonoTimer(OWN_TASK, MQTT_PING_TIME, T_MQTT_BROKER_DEAD); // monitor the broker's ping response so that we can try again later
                }
                else {
                    ucMQTT_state = MQTT_STATE_CONNECTED_IDLE;            // cancel the present state
                    fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);      // cause a TCP disconnection because the broker is not behaving
                }
                break;
            }
        }
    }
}


// The user calls this to initiate a connection to the MQTT server/broker
//
extern int fnConnectMQTT(unsigned char *ucIP, unsigned short(*fnCallback)(unsigned char, unsigned char *, unsigned long, unsigned char))
{
    if (MQTT_TCP_socket < 0) {                                           // we have no socket - or called before initialisation complete    
        if ((MQTT_TCP_socket = fnGetTCP_Socket(TOS_MINIMISE_DELAY, INFINITE_TIMEOUT, fnMQTTListener)) < 0) {
            return ERROR_MQTT_NOT_READY;
        }
    }

    if (ucMQTT_state != MQTT_STATE_CLOSED) {
        return ERROR_MQTT_IN_USE;                                        // called while already active
    }

    fnUserCallback = fnCallback;
    uMemcpy(ucMQTT_ip_address, ucIP, IPV4_LENGTH);                       // save the address of the MQTT server/broker we want to connect to

#if 0 // temp
    ucMQTT_state = MQTT_STATE_CONNECTED_IDLE;
    fnMQTTListener(MQTT_TCP_socket, TCP_EVENT_DATA, test, sizeof(test));
    fnMQTTListener(MQTT_TCP_socket, TCP_EVENT_DATA, test2, sizeof(test2));
#else
    fnSetNextMQTT_state(MQTT_STATE_OPEN_REQUESTED);
#endif
    return 0;                                                            // OK    
}

// The user calls this to disconnect from the MQTT server/broker
//
extern int fnDisconnectMQTT(void)
{
    if ((MQTT_TCP_socket < 0) || (ucMQTT_state < MQTT_STATE_CONNECTION_OPENED)) { // we have no socket - or called when not connected
        return ERROR_MQTT_NOT_READY;
    }
    ucQueueFlags = MQTT_QUEUE_CLOSE;                                     // mark that we want to close
    if ((ucMQTT_state & 0x01) == 0) {                                    // not busy with other commands (otherwise it will be executed at next possible opportunity)
        fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);                  // this will cause the connection to close due to the pending close flag
    }
    return 0;                                                            // disconnection in progress or queued
}

// The user can subscribe to a topic at any time when connected, but should not subscribe to futher topics until the previous one has been acknowledged
//
extern int fnSubscribeMQTT(CHAR *ptrInput, unsigned char ucQoS)
{
    int iSubscriptionRef = 0;
    if ((MQTT_TCP_socket < 0) || (ucMQTT_state < MQTT_STATE_CONNECTION_OPENED)) { // we have no socket - or called when not connected
        return ERROR_MQTT_NOT_READY;
    }
    
    FOREVER_LOOP() {
        if (subscriptions[iSubscriptionRef].ucSubscriptionReference == 0) { // empty subscription found
            break;                                                       // use this entry
        }
        if (++iSubscriptionRef > MQTT_MAX_SUBSCRIPTIONS) {
            return ERROR_MQTT_NO_SUBSCRIPTION_ENTRY;                     // all subscription entries are in use
        }
    }
    if (ucQoS > MQTT_SUBSCRIPTION_QoS_2) {                               // limit QoS to valid range
        ucQoS = MQTT_SUBSCRIPTION_QoS_2;
    }
    subscriptions[iSubscriptionRef].ucSubscriptionQoS = ucQoS;
    subscriptions[iSubscriptionRef].ucSubscriptionReference = ucSubscriptionInProgress = (unsigned char)(iSubscriptionRef + 1);
    uStrcpy(subscriptions[iSubscriptionRef].cSubscriptionTopic, ptrInput); // enter the topic string
    if ((ucMQTT_state & 0x01) == 0) {                                    // not busy with other commands (otherwise it will be executed at next possible opportunity)
        fnSetNextMQTT_state(MQTT_STATE_SUBSCRIBE);                       // send
    }
    else {
        ucQueueFlags |= MQTT_QUEUE_SUBSCRIBE;
    }
    return ucSubscriptionInProgress;                                     // either sent or queued to be sent
}

extern int fnUnsubscribeMQTT(unsigned char ucSubscriptionRef)
{
    int iSubscriptionRef = 0;
    FOREVER_LOOP() {
        if (subscriptions[iSubscriptionRef].ucSubscriptionReference == ucSubscriptionRef) { // subscription found
            break;                                                       // use this entry
        }
        if (++iSubscriptionRef > MQTT_MAX_SUBSCRIPTIONS) {
            return ERROR_MQTT_NO_SUBSCRIPTION_ENTRY;                     // all subscription entries are in use
        }
    }
    ucSubscriptionInProgress = ucSubscriptionRef;
    if ((ucMQTT_state & 0x01) == 0) {                                    // not busy with other commands (otherwise it will be executed at next possible opportunity)
        fnSetNextMQTT_state(MQTT_STATE_UNSUBSCRIBE);                     // send
    }
    else {
        ucQueueFlags |= MQTT_QUEUE_UNSUBSCRIBE;
    }
    return 0;                                                             // either sent or queued to be sent
}

// The user can publish at any time when connected, but should not publish further messages until the previous one has been acknowledged
//
extern int fnPublishMQTT(unsigned char ucTopicReference, unsigned char ucQoS)
{
    int iSubscriptionRef = 0;
    if ((MQTT_TCP_socket < 0) || (ucMQTT_state < MQTT_STATE_CONNECTION_OPENED)) { // we have no socket - or called when not connected
        return ERROR_MQTT_NOT_READY;
    }
    
    FOREVER_LOOP() {
        if ((ucTopicReference != 0) && (subscriptions[iSubscriptionRef].ucSubscriptionReference == ucTopicReference)) { // subscription found
            ucPublishQoS = subscriptions[iSubscriptionRef].ucSubscriptionQoS; // inherit the QoS from the subscription
            ucPublishInProgress = ucTopicReference;
            break;                                                       // use this entry
        }
        if (++iSubscriptionRef > MQTT_MAX_SUBSCRIPTIONS) {
            ucPublishInProgress = 0;                                     // no subscription topic found so use callback for topic
            if (ucQoS > MQTT_SUBSCRIPTION_QoS_2) {                       // limit QoS to valid range
                ucQoS = MQTT_SUBSCRIPTION_QoS_2;
            }
            ucPublishQoS = ucQoS;
            break;
        }
    }

    if ((ucMQTT_state & 0x01) == 0) {                                    // not busy with other commands (otherwise it will be executed at next possible opportunity)
        fnSetNextMQTT_state(MQTT_STATE_PUBLISH);                         // send
    }
    else {
        ucQueueFlags |= MQTT_QUEUE_PUBLISH;
    }
    return 0;                                                            // either sent or queued to be sent
}

// After sending a publish message (with QoS2) the broker has returned a public received response, to which a publich release to sent or queued (expect a pubish complete to terminate)
//
static int fnSendPublishReceived(void)
{
    if ((ucMQTT_state & 0x01) == 0) {                                    // not busy with other commands (otherwise it will be executed at next possible opportunity)
        return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH_RECEIVED));       // send
    }
    else {
        ucQueueFlags |= MQTT_QUEUE_PUBLISH_RECEIVED;                     // queue
    }
    return 0;                                                            // queued
}

static int fnSendPublishAck(void)
{
    if ((ucMQTT_state & 0x01) == 0) {                                    // not busy with other commands (otherwise it will be executed at next possible opportunity)
        return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH_ACK));            // send
    }
    else {
        ucQueueFlags |= MQTT_QUEUE_PUBLISH_ACK;                          // queue
    }
    return 0;                                                            // queued
}

// Send a PINGREQ to signal that we are still here
//
static void fnMQTT_ping(void)
{
    if (ucMQTT_state == MQTT_STATE_CONNECTED_IDLE) {                     // if the state is not idle we ignore the request to send a ping since other activity will be in progress
        fnSetNextMQTT_state(MQTT_STATE_SENDING_KEEPALIVE);
    }
}

// Moe to the next MQTT state
//
static int fnSetNextMQTT_state(unsigned char ucNextState)
{
    if (ucNextState == MQTT_STATE_CONNECTED_IDLE) {                      // if going idle move to possible queued states, handled in the order of priority that they are checked in
        if ((ucQueueFlags & MQTT_QUEUE_PUBLISH_ACK) != 0) {              // if a publish ack is queued
            ucQueueFlags &= ~(MQTT_QUEUE_PUBLISH_ACK);                   // no longer queued
            ucNextState = MQTT_STATE_PUBLISH_ACK;                        // start state publish ack
        }
        else if ((ucQueueFlags & MQTT_QUEUE_PUBLISH_RECEIVED) != 0) {    // if a publish received is queued
            ucQueueFlags &= ~(MQTT_QUEUE_PUBLISH_RECEIVED);              // no longer queued
            ucNextState = MQTT_STATE_PUBLISH_RECEIVED;                   // start state publish received
        }
        else if ((ucQueueFlags & MQTT_QUEUE_PUBLISH) != 0) {             // if a publish request is queued
            ucQueueFlags &= ~(MQTT_QUEUE_PUBLISH);
            ucNextState = MQTT_STATE_PUBLISH;                            // start state publish
        }
        else if ((ucQueueFlags & MQTT_QUEUE_UNSUBSCRIBE) != 0) {         // if a unsubscribe request is queued
            ucQueueFlags &= ~(MQTT_QUEUE_SUBSCRIBE);
            ucNextState = MQTT_STATE_UNSUBSCRIBE;                        // start state unsubscribe
        }
        else if ((ucQueueFlags & MQTT_QUEUE_SUBSCRIBE) != 0) {           // if a subscribe request is queued
            ucQueueFlags &= ~(MQTT_QUEUE_SUBSCRIBE);
            ucNextState = MQTT_STATE_SUBSCRIBE;                          // start state publish
        }
    }
    switch (ucMQTT_state = ucNextState) {                                // set the next state
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
          uMemset(&subscriptions[0], 0, sizeof(subscriptions));
          uTaskerStopTimer(OWN_TASK);
          break;
      case MQTT_STATE_PUBLISH_ACK:                                       // moving to states that require a message to be sent
      case MQTT_STATE_PUBLISH_COMPLETE:
      case MQTT_STATE_PUBLISH_RECEIVED:
      case MQTT_STATE_SENDING_KEEPALIVE:
      case MQTT_STATE_CONNECTION_OPENED:
      case MQTT_STATE_SUBSCRIBE:
      case MQTT_STATE_UNSUBSCRIBE:
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

// Local listener on TCP MQTT port
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

    case TCP_EVENT_CLOSE:                                                // broker is requesting a TCP close
        fnSetNextMQTT_state(MQTT_STATE_CLOSING);
        break;

    case TCP_EVENT_ABORT:                                                // broker has reset the TCP connection
        fnMQTT_error(MQTT_HOST_CLOSED);
        break;

    case TCP_EVENT_CLOSED:                                               // the TCP connection has closed
        fnSetNextMQTT_state(MQTT_STATE_CLOSED);
        fnUserCallback(MQTT_CONNECTION_CLOSED, 0, 0, 0);
        break;

    case TCP_EVENT_ACK:                                                  // last TCP transmission has been acknowledged
        ucUnacked = 0;                                                   // no more outstanding data to be acked
        if ((MQTT_STATE_PUBLISH_COMPLETE == ucMQTT_state) || (MQTT_CONTROL_PACKET_TYPE_PUBACK == ucMQTT_state)) {
            ucMQTT_state = MQTT_STATE_CONNECTED_IDLE;
        }
        if ((ucQueueFlags & MQTT_QUEUE_REGEN) == 0) {
            break;
        }
        ucQueueFlags &= ~MQTT_QUEUE_REGEN;                               // a transmission was not possible due to outstanding ack so we now allow it to be relesed
        // Fall through intentionally
        //
    case TCP_EVENT_REGENERATE:                                           // we must repeat the previous transmission
        ucUnacked = 0;
        return (fnRegenerate() > 0);

    case TCP_EVENT_DATA:                                                 // we have new receive data
        return (fnHandleData(ucIp_Data, usPortLen));                     // interpret the data

    case TCP_EVENT_CONREQ:                                               // we do not accept connection requests
    default:
        return -1;
    }

    return APP_ACCEPT;
}

// The remaining length field is encoded to one to four bytes - we support just two (0..16383 range)
//
static unsigned short fnAddMQTT_remaining_length(unsigned char *ptrEnd, unsigned char *ptrStart, int iMaxLength, unsigned char *ptrRemainingLength, int *iVarLenInsert)
{
    unsigned long ulRemainingLength = ((ptrEnd - ptrRemainingLength) - 1);
    if (ulRemainingLength <= 127) {                              
        *ptrRemainingLength = (unsigned char)ulRemainingLength;          // single byte to represent the length
    }
    else {                                                               // we need to encode the length to two bytes
        uMemcpy((ptrStart - 1), ptrStart, (ptrRemainingLength - ptrStart)); // rather than shift the message content to make space we shift the small header instead
        *iVarLenInsert = 0;
        ptrStart--;
        *ptrRemainingLength = (unsigned char)(ulRemainingLength/128);
        ulRemainingLength = ulRemainingLength - (*ptrRemainingLength * 128);
        ptrRemainingLength--;
        *ptrRemainingLength = (unsigned char)(0x80 | ulRemainingLength);
    }
    return (ptrEnd - ptrStart);                                          // length
}

// Insert the user's string into the message
//
unsigned char *fnMQTTUserString(unsigned char *ptrMQTT_packet, unsigned char ucEvent)
{
    unsigned char *ptrStringLength = ptrMQTT_packet;
    unsigned short usStringLength;
    ptrMQTT_packet += 2;                                                 // leave space for the string length
    usStringLength = fnUserCallback(ucEvent, ptrMQTT_packet, 0, 0);      // request user to add the string
    *ptrStringLength++ = (unsigned char)(usStringLength >> 8);           // insert the length before the string content
    *ptrStringLength++ = (unsigned char)(usStringLength);
    return (ptrMQTT_packet + usStringLength);
}

// Insert a topic string into the message
//
unsigned char *fnMQTTTopicString(unsigned char *ptrMQTT_packet, CHAR *ptrString)
{
    unsigned char *ptrStringLength = ptrMQTT_packet;
    unsigned short usStringLength;
    ptrMQTT_packet += 2;                                                 // leave space for the string length
    ptrMQTT_packet = (unsigned char *)uStrcpy((CHAR *)ptrMQTT_packet, ptrString);
    usStringLength = ((ptrMQTT_packet - (unsigned char *)ptrStringLength) - 2);
    *ptrStringLength++ = (unsigned char)(usStringLength >> 8);           // insert the length before the string content
    *ptrStringLength++ = (unsigned char)(usStringLength);
    return (ptrMQTT_packet);
}

// Insert the identifier into the message
//
unsigned char *fnInsertPacketIdentfier(unsigned char *ptrMQTT_packet, unsigned short usIdentifier)
{
    *ptrMQTT_packet++ = (unsigned char)(usIdentifier >> 8);
    *ptrMQTT_packet++ = (unsigned char)(usIdentifier);
    return ptrMQTT_packet;
}

// Increment the packet idetifier so that each transaction has a unique number, but avoiding the value 0
//
static void fnIncrementtPacketIdentfier(unsigned char ucControlPacketType)
{
    if (++usPacketIdentifier == 0) {                                     // avoid invalid value of zero
        usPacketIdentifier = 1;
    }
}

// Construct and send a message (either for the first time or in order to repeat a lost transmission
//
static unsigned short fnRegenerate(void)
{
    unsigned char ucMQTTData[MIN_TCP_HLEN + MQTT_MESSAGE_LEN + 1];
    unsigned short usDataLen = 0;
    unsigned char *ptrMQTT_packet = (unsigned char *)&ucMQTTData[MIN_TCP_HLEN + 1]; // leave one byte at the start free in case we need to add a two byte variable length
    unsigned char *ptrRemainingLength;
    int iVarLenInsert = 1;

    if (ucUnacked != 0) {                                                // if there is unacked data we need to wait until it has been acked before we can continue
        ucQueueFlags |= MQTT_QUEUE_REGEN;                                // flag that we want to continue as soon as the outstanding TCP data has been acknowleged
        return 0;
    }

    switch (ucMQTT_state) {                                              // resent last packet
    case MQTT_STATE_CONNECTION_OPENED:                                   // client to broker after establishing the TCP connection
        // The TCP connection has been established so we now request an MQTT connection
        //
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_CONNECT | 0);      // flags are not used by the connection connection request
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        uMemcpy(ptrMQTT_packet, cucProtocolNameMQTT, sizeof(cucProtocolNameMQTT)); // add the variable header, beginning with the fixed protocol name
        ptrMQTT_packet += sizeof(cucProtocolNameMQTT);
        *ptrMQTT_packet++ = MQTT_PROTOCOL_LEVEL;
        *ptrMQTT_packet++ = MQTT_CONNECT_FLAG_CLEAN_SESSION;             // clear any previous session states
        *ptrMQTT_packet++ = (unsigned char)(MQTT_KEEPALIVE_TIME_SECONDS >> 8); // keep-alive time (the broker should disconnect when there is no acivity during this interval)
        *ptrMQTT_packet++ = (unsigned char)(MQTT_KEEPALIVE_TIME_SECONDS);
        // Payload
        //
        ptrMQTT_packet = fnMQTTUserString(ptrMQTT_packet, MQTT_CLIENT_IDENTIFIER); // callback to get the client idetifier (should normally contain only 0..9, a..z, A..Z characters)
        break;

    case MQTT_STATE_UNSUBSCRIBE:
    case MQTT_STATE_SUBSCRIBE:
        if (MQTT_STATE_UNSUBSCRIBE == ucMQTT_state) {
            *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_UNSUBSCRIBE | MQTT_CONTROL_PACKET_FLAG_QoS_1); // the QoS is always 1 for an unsubscription
        }
        else {
            *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_SUBSCRIBE | MQTT_CONTROL_PACKET_FLAG_QoS_1); // the QoS is always 1 for a subscription
        }
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, usPacketIdentifier);
        // Topic filter
        //
        if (ucSubscriptionInProgress == 0) {
            return 0;
        }
        ptrMQTT_packet = fnMQTTTopicString(ptrMQTT_packet, subscriptions[ucSubscriptionInProgress - 1].cSubscriptionTopic);
        if (MQTT_STATE_SUBSCRIBE == ucMQTT_state) {
            *ptrMQTT_packet++ = subscriptions[ucSubscriptionInProgress - 1].ucSubscriptionQoS; // the subscription QoS
        }
        break;

    case MQTT_STATE_PUBLISH:                                             // client to broker or broker to client - transport an application message
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_PUBLISH | (ucPublishQoS << 1));
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        // Topic name
        //
        if (ucPublishInProgress == 0) {
            ptrMQTT_packet = fnMQTTUserString(ptrMQTT_packet, MQTT_PUBLISH_TOPIC); // request the topic from the user
        }
        else {
            ptrMQTT_packet = fnMQTTTopicString(ptrMQTT_packet, subscriptions[ucPublishInProgress - 1].cSubscriptionTopic);
            ucPublishInProgress = subscriptions[ucPublishInProgress - 1].ucSubscriptionReference;
        }

        if (ucPublishQoS > 0) {                                          // insert our packet identifier if a QoS greater that 0 is used
            // Packet identifier - only present when QoS 1 or 2 is used! Must be non-zero - the respone needs to match!
            //
            ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, usPacketIdentifier);
        }

        // Application data
        //
        ptrMQTT_packet += fnUserCallback(MQTT_PUBLISH_DATA, ptrMQTT_packet, 0, ucPublishInProgress); // request user to insert the message content - can be any format and length, including zero length
        break;

    case MQTT_STATE_PUBLISH_RELEASE:
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_PUBREL | MQTT_CONTROL_PACKET_FLAG_QoS_1);
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, usPacketIdentifier);
        break;

    case MQTT_STATE_PUBLISH_ACK:
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_PUBACK);
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, usMessageIdentifier);
        break;

    case MQTT_STATE_PUBLISH_RECEIVED:
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_PUBREC);
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, usMessageIdentifier);
        break;

    case MQTT_STATE_PUBLISH_COMPLETE:
        *ptrMQTT_packet++ = (MQTT_CONTROL_PACKET_TYPE_PUBCOMP);
        ptrRemainingLength = ptrMQTT_packet++;                           // the location where the remaining length is to be added
        ptrMQTT_packet = fnInsertPacketIdentfier(ptrMQTT_packet, usMessageIdentifier);
        break;

    case MQTT_STATE_SENDING_KEEPALIVE:
        *ptrMQTT_packet++ = MQTT_CONTROL_PACKET_TYPE_PINGREQ;            // ping request
        *ptrMQTT_packet++ = 0;
        uTaskerMonoTimer(OWN_TASK, MQTT_PING_TIME, T_MQTT_BROKER_DEAD);  // monitor the broker's ping response
        return (ucUnacked = (fnSendTCP(MQTT_TCP_socket, (ucMQTTData + 1), 2, TCP_FLAG_PUSH) > 0)); // send data

    default:
        return 0;
    }
    if (usDataLen == 0) {
        usDataLen = fnAddMQTT_remaining_length(ptrMQTT_packet, (unsigned char *)&ucMQTTData[MIN_TCP_HLEN + 1], MQTT_MESSAGE_LEN, ptrRemainingLength, &iVarLenInsert);
    }
    uTaskerMonoTimer(OWN_TASK, MQTT_KEEPALIVE_TIME, T_MQTT_KEEPALIVE_TIMEOUT); // retrigger the keep-alive timer at each transmission
    return (ucUnacked = (fnSendTCP(MQTT_TCP_socket, (ucMQTTData + iVarLenInsert), usDataLen, TCP_FLAG_PUSH) > 0)); // send data
}

// Handle receptions from the broker
//
static int fnHandleData(unsigned char *ptrData, unsigned short usDataLength)
{
    unsigned char ucControlPacketType = (*ptrData & MQTT_CONTROL_PACKET_TYPE_MASK);
    if (ucMQTT_state >= MQTT_STATE_CONNECTION_OPENED) {
        // Reception that is state-independend as long as there is an open connection
        //
        switch (ucControlPacketType) {
        case MQTT_CONTROL_PACKET_TYPE_PUBLISH:                           // broker is publishing data - presumably to a topic that we have subscribed to
            {
                int iSubscriptionRef = 0;
                unsigned char ucFlags = *ptrData++;
                unsigned long ulLength = 0;
                int iMultiplier = 1;
                unsigned short usTopicLength;
                unsigned char ucEncodedByte;
                FOREVER_LOOP() {
                    ucEncodedByte = *ptrData++;
                    ulLength += ((ucEncodedByte & 0x7f) * iMultiplier);
                    if ((ucEncodedByte & 0x80) == 0) {
                        break;
                    }
                    if (iMultiplier >= (128 * 128)) {
                        return 0;                                        // malformed remaining length
                    }
                    iMultiplier *= 128;
                }
                usTopicLength = *ptrData++;
                usTopicLength <<= 8;
                usTopicLength |= *ptrData++;

                if ((MQTT_TCP_socket < 0) || (ucMQTT_state < MQTT_STATE_CONNECTION_OPENED)) { // we have no socket - or called when not connected
                    return ERROR_MQTT_NOT_READY;
                }

                FOREVER_LOOP() {
                    if (subscriptions[iSubscriptionRef].ucSubscriptionReference != 0) {
                        if (uMemcmp(subscriptions[iSubscriptionRef].cSubscriptionTopic, ptrData, usTopicLength) == 0) {
                            break;                                                   // use this entry
                        }
                    }
                    if (++iSubscriptionRef > MQTT_MAX_SUBSCRIPTIONS) {
                        return 0;                                        // no subscription found for this topic so ignore it
                    }
                }
                ptrData += usTopicLength;
                if ((ucFlags & MQTT_CONTROL_PACKET_FLAG_QoS) == MQTT_CONTROL_PACKET_FLAG_QoS_2) {
                    usMessageIdentifier = *ptrData++;
                    usMessageIdentifier <<= 8;
                    usMessageIdentifier |= *ptrData++;
                    ulLength -= 2;
                }
                ulLength -= (2 + usTopicLength);
                fnUserCallback(MQTT_TOPIC_MESSAGE, ptrData, ulLength, subscriptions[iSubscriptionRef].ucSubscriptionReference); // inform the user of the topic's message content and length
                if ((ucFlags & MQTT_CONTROL_PACKET_FLAG_RETAIN) != 0) {  // ??
                }
                if ((ucFlags & MQTT_CONTROL_PACKET_FLAG_DUP) != 0) {     // ??
                }
                switch (ucFlags & MQTT_CONTROL_PACKET_FLAG_QoS) {
                case MQTT_CONTROL_PACKET_FLAG_QoS_0:
                default:
                    return 0;                                            // no handshake stage
                case MQTT_CONTROL_PACKET_FLAG_QoS_1:
                    return (fnSendPublishAck());                         // respond with public ack to terminate
                case MQTT_CONTROL_PACKET_FLAG_QoS_2:
                    return (fnSendPublishReceived());                    // respond with public received, after which a public release si expected, which will be terminated with public complete
                }
            }
            break;
        default:
            break;
        }
    }

    switch (ucMQTT_state) {
    case MQTT_STATE_CONNECTION_OPENED:
        if (ucControlPacketType == MQTT_CONTROL_PACKET_TYPE_CONNACK) {   // the broker is accepting the MQTT connection
            fnUserCallback(MQTT_CONNACK_RECEIVED, 0, 0, 0);              // inform the user that the broker has accepted
            fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);
        }
        else {

        }
        break;
    case MQTT_STATE_SUBSCRIBE:
        if (ucControlPacketType == MQTT_CONTROL_PACKET_TYPE_SUBACK) {    // the broker is accepting the subscription
            fnIncrementtPacketIdentfier((MQTT_CONTROL_PACKET_TYPE_SUBSCRIBE >> 4));
            fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);
            fnUserCallback(MQTT_SUBACK_RECEIVED, 0, 0, subscriptions[ucSubscriptionInProgress - 1].ucSubscriptionReference);
        }
        else {

        }
        break;
    case MQTT_STATE_UNSUBSCRIBE:
        if (ucControlPacketType == MQTT_CONTROL_PACKET_TYPE_UNSUBACK) {  // the broker is accepting the unsubscription
            fnIncrementtPacketIdentfier((MQTT_CONTROL_PACKET_TYPE_UNSUBSCRIBE >> 4));
            subscriptions[ucSubscriptionInProgress - 1].ucSubscriptionReference = 0;
            fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);
            fnUserCallback(MQTT_UNSUBACK_RECEIVED, 0, 0, subscriptions[ucSubscriptionInProgress - 1].ucSubscriptionReference);
        }
        else {

        }
        break;
    case MQTT_STATE_PUBLISH:
        if (ucControlPacketType == MQTT_CONTROL_PACKET_TYPE_PUBREC) {    // the broker is accepting to publish
            if (usDataLength == 4) {
                if (((unsigned char)(usPacketIdentifier >> 8) == *(ptrData + 2)) && (((unsigned char)(usPacketIdentifier) == *(ptrData + 3)))) {
                    if (ucPublishQoS == MQTT_SUBSCRIPTION_QoS_2) {
                        return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH_RELEASE)); // send the third packet of QoS 2 protocol exchange
                    }
                    fnIncrementtPacketIdentfier((MQTT_CONTROL_PACKET_TYPE_PUBLISH >> 4));
                    fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);
                    fnUserCallback(MQTT_PUBLISH_RECEIVED, 0, 0, subscriptions[ucPublishInProgress - 1].ucSubscriptionReference);
                }
            }
        }
        else {

        }
        break;
    case MQTT_STATE_PUBLISH_RELEASE:
        if (ucControlPacketType == MQTT_CONTROL_PACKET_TYPE_PUBCOMP) {   // the broker is accepting to publish - handshake stage
            if (usDataLength == 4) {
                if (((unsigned char)(usPacketIdentifier >> 8) == *(ptrData + 2)) && (((unsigned char)(usPacketIdentifier) == *(ptrData + 3)))) {
                    fnIncrementtPacketIdentfier((MQTT_CONTROL_PACKET_TYPE_PUBLISH >> 4));
                    if (fnUserCallback(MQTT_PUBLISH_RECEIVED, 0, 0, subscriptions[ucPublishInProgress - 1].ucSubscriptionReference) != 0) { // inform the user that the broker has accepted to publish
                        return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH));// user wants to immediately publish a new message
                    }
                    fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);
                }
            }
        }
        else {

        }
        break;
    case MQTT_STATE_PUBLISH_RECEIVED:
        if (ucControlPacketType == MQTT_CONTROL_PACKET_TYPE_PUBREL) {    // the broker is publishing - handshake stage
            if (usDataLength == 4) {
                if (((unsigned char)(usMessageIdentifier >> 8) == *(ptrData + 2)) && (((unsigned char)(usMessageIdentifier) == *(ptrData + 3)))) {
                    return (fnSetNextMQTT_state(MQTT_STATE_PUBLISH_COMPLETE)); // complete the pubish handshake
                }
            }
        }
        else {

        }
        break;
    case MQTT_STATE_SENDING_KEEPALIVE:
        if (ucControlPacketType == MQTT_CONTROL_PACKET_TYPE_PINGACK) {   // the broker is replying with a ping response
            fnSetNextMQTT_state(MQTT_STATE_CONNECTED_IDLE);
            uTaskerMonoTimer(OWN_TASK, MQTT_KEEPALIVE_TIME, T_MQTT_KEEPALIVE_TIMEOUT); // retrigger the keep-alive timer
        }
        break;
    }
    return 0;
}


static void fnMQTT_error(unsigned char ucError)
{
    fnSetNextMQTT_state(MQTT_STATE_CLOSED);
    fnUserCallback(ucError, 0, 0, 0);
}
#endif
