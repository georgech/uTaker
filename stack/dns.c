/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      dns.c (DNS client)
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    26.12.2009 Add random ID to ensure that no routers block repeated ID values {1}
    09.12.2013 Correction of DNS query length and allow a host name to end with '.' (thanks to Jakob M.N) {2}

*/        

#include "config.h"


#if defined USE_DNS || defined USE_mDNS_SERVER

#define DNS_BUFFER             ((263 + 13 + 5) - sizeof(UDP_HEADER))

typedef struct stUDP_DNS_MESSAGE
{     
    UDP_HEADER     tUDP_Header;                                          // reserve header space
    unsigned char  ucUDP_Message[DNS_BUFFER];
} UDP_DNS_MESSAGE;
#endif

#if defined USE_mDNS_SERVER
    static const unsigned char ucMulticastIP[IPV4_LENGTH] = {224, 0, 0, 251}; // link-local multicast IPv4 address
#endif

#if defined USE_DNS

#define OWN_TASK               TASK_DNS

#define DNS_RESEND_PERIOD     (DELAY_LIMIT)(2 * SEC)

#define E_DNS_RESEND           1

#define DNS_STATE_INIT         0
#define DNS_STATE_READY        1
#define DNS_STATE_BUSY         2
#define DNS_STATE_RESEND       3


#define DNS_NUM_RETRIES        5


#define DNS_ERROR_BUSY        -1
#define DNS_ERROR_LABEL       -2
#define DNS_ERROR_NAME        -3
#define DNS_ERROR_OVERFLOW    -4


static const unsigned char ucZeroQTypeClass[] = {0x00, 0x00, 0x01, 0x00, 0x01};
static const unsigned char ucTypeClass[]      = {0x00, 0x01, 0x00, 0x01};

static unsigned char ucDNS_server_temp_ip[IPV4_LENGTH];

static int  fnDNSListner(USOCKET dns_socket, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen);
static int  fnRetry(void);
static void fnFindAnswer(unsigned char *ucData, unsigned short usDNS_nscount, unsigned short usDNS_ancount);
static void fnDNS_error(unsigned char ucError);


static unsigned char ucDNS_state = DNS_STATE_INIT;
static USOCKET DNSSocketNr = -1;                                         // UDP socket number
static unsigned char ucDNSRetries;
static void (*fnClientListener)(unsigned char ucEvent, unsigned char *ucIP);
static const CHAR *ptrDNS_host_name;
#if defined RANDOM_NUMBER_GENERATOR                                           // {1}
    static unsigned short usRequestID;
#endif

// DNS task
//
void fnDNS(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if (ucDNS_state == 0) {
        ucDNS_state = DNS_STATE_READY;
        if (((DNSSocketNr = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnDNSListner, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS))) >= 0)) {
            fnBindSocket(DNSSocketNr, DNS_UDP_PORT);
        }    
    } 
    else {
        if (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) {// check task input queue
            if (ucInputMessage[MSG_SOURCE_TASK] == TASK_ARP) {
                fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]);  // read the contents
                if (ARP_RESOLUTION_FAILED == ucInputMessage[0]) {
                    if (fnRetry()) {                                     // DNS server could not be resolved - try a few times before informing listener  
                        fnDNS_error(DNS_ERROR_NO_ARP_RES);
                    }
                }
                else if (ARP_RESOLUTION_SUCCESS == ucInputMessage[0]) {
                    ucDNSRetries = (DNS_NUM_RETRIES + 1);                // the first request was lost due to ARP resolution
                    fnRetry();                                           // the ARP resolution was successful - now start fresh
                }
            }
            else if (ucInputMessage[MSG_SOURCE_TASK] == TIMER_EVENT) {
                if (fnRetry() != 0) {                                    // resolution timeout (E_DNS_RESEND)
                    fnDNS_error(DNS_ERROR_TIMEOUT);
                }
            }
        }
    }
}

// Resend a DNS query
//
static int fnRetry(void)
{
    if (ucDNSRetries != 0) {
        ucDNS_state = DNS_STATE_RESEND;
        ucDNSRetries--;
        fnResolveHostName(ptrDNS_host_name, fnClientListener);           // retry
    }
    else {                                                               // last timeout - resolution failed
        ucDNS_state = DNS_STATE_READY;
        return 1;                                                        // we give up
    }
    return 0;                                                            // OK - repetition attempted
}

// Send a DNS query to resolve the address of specified host name
//
extern signed short fnResolveHostName(const CHAR *cHostNamePtr, void (*fnListener)(unsigned char , unsigned char *))
{
    unsigned int    i;
    unsigned short  usTotal;
    UDP_DNS_MESSAGE UDP_Message;
    unsigned char  *ucBuf;

    switch (ucDNS_state) {
        case DNS_STATE_READY:
            ucDNS_state = DNS_STATE_BUSY;
            ptrDNS_host_name = cHostNamePtr;                             // remember the name we want to resolve (caller must keep this valid)
            fnClientListener = fnListener;                               // save the client listener function   
            uMemcpy(ucDNS_server_temp_ip, &network[DEFAULT_NETWORK].ucDNS_server[0], IPV4_LENGTH);
            ucDNSRetries = DNS_NUM_RETRIES;
            break;

        case DNS_STATE_BUSY:                                             // we support only one resolve at a time so return with error
            return DNS_ERROR_BUSY;

        case DNS_STATE_RESEND:
            if (ptrDNS_host_name != cHostNamePtr) {
                return DNS_ERROR_BUSY;
            }
            break;

        default:
            return DNS_ERROR_BUSY;
    }
    ucBuf = UDP_Message.ucUDP_Message;
    uMemset(ucBuf, 0, DNS_BUFFER);                                       // ensure content zeroed

#if defined RANDOM_NUMBER_GENERATOR                                           // {1}
    usRequestID = fnRandom();
    *ucBuf++ = (unsigned char)(usRequestID);                             // give out request a transaction ID for verifying the corresponding response
    *ucBuf++ = (unsigned char)(usRequestID >> 8);
#else
    *ucBuf++ = 0xaa;                                                     // now create the message
    *ucBuf++ = 0xaa;                                                     // a fixed transaction ID
#endif
    *ucBuf++ = 0x01;
    *ucBuf++ = 0x00;
    *ucBuf++ = 0x00;                                                     // single question count
    *ucBuf++ = 0x01;

    ucBuf += 6;                                                          // other entries left at zero

    usTotal = 12;                                                        // create the question section {2} initial length

    while ((*cHostNamePtr) != '\0') {                                    // wile the end of the host server name string has not been encountered
        ucBuf++;                                                         // reserve space for count
        i = 0;

        while (((*cHostNamePtr) != '.') && ((*cHostNamePtr) != '\0')) {  // whiel neither dot nor end of string
            if (++i >= (DNS_BUFFER - usTotal/* - 13*/)) {                // ensure that there is adequate space in the DNS buffer
                return (DNS_ERROR_OVERFLOW);                             // buffer overflow (not enough buffer space available for name)
            }
            *ucBuf++ = *(unsigned char *)cHostNamePtr++;                 // copy name
        }

        if ((i == 0) || (i > 63)){                                       // label doesn't exist or longer than 63 bytes?
            return(DNS_ERROR_LABEL);
        }

        usTotal += (i + 1);                                              // total name length

        if (usTotal >= (264 + 12)) {                                     // {2} check for maximum length
            return(DNS_ERROR_NAME);
        }

        *(ucBuf - i - 1) = (unsigned char)i;                             // store label length

        if ((*cHostNamePtr) == '.') {                                    // still not at the end, skip dot
            cHostNamePtr++;
        }
      //else {                                                           // {2}
        if ((*cHostNamePtr) == '\0') {                                   // add ZERO, QTYPE and QCLASS
            uMemcpy(ucBuf, ucZeroQTypeClass, sizeof(ucZeroQTypeClass));
            usTotal += sizeof(ucZeroQTypeClass);
            uTaskerMonoTimer(OWN_TASK, DNS_RESEND_PERIOD, E_DNS_RESEND); // monitor resolution
                                                                         // send the request
            fnSendUDP(DNSSocketNr, ucDNS_server_temp_ip, DNS_UDP_PORT, (unsigned char *)&UDP_Message.tUDP_Header, (unsigned short)(usTotal/* + 13 + sizeof(ucZeroQTypeClass)*/), OWN_TASK); // {2} (13 was originally one byte too many which could cause rejection by certain DNS servers)
        }
      //}
    }
    return 0;
}

// The local DNS listener function - this will call the client listener if needed
//
static int fnDNSListner(USOCKET dns_socket, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen)
{
    unsigned short  usDNS_nscount, usDNS_ancount;
    const CHAR     *cTmpPtr;
    unsigned char   ucLabLen;

    if ((DNSSocketNr != dns_socket) || (ucDNS_state == DNS_STATE_READY) || (uMemcmp(ucIP, ucDNS_server_temp_ip, IPV4_LENGTH))) {
        return (-1);                                                     // not our socket, or not in waiting state or bad IP
    }

    if (ucEvent == UDP_EVENT_RXDATA) {
        if (usDataLen < (16 - UDP_HLEN)) {
            return -1;                                                   // too short for DNS reply
        }
#if defined RANDOM_NUMBER_GENERATOR                                      // {1}
        if ((*ucData++ != (unsigned char)(usRequestID)) || (*ucData++ != (unsigned char)(usRequestID >> 8))) {
            return -1;                                                   // wrong ID
        }
#else
        if ((*ucData++ != 0xaa) || (*ucData++ != 0xaa)) {
            return -1;                                                   // wrong ID
        }
#endif
        if (!(*ucData & 0x80)) {
            return -1;                                                   // not an answer - we are not DNS server...
        }
        if ((*ucData++ & 0x7a)) {
            fnDNS_error(DNS_ERROR_GENERAL);
            return -1;                                                   // op code not zero or message truncated
        }
        if ((*ucData & 0x7f) && (*ucData & 0x80)) {
            fnDNS_error(DNS_OPCODE_ERROR);
            return -1;                                                   // op code not zero with recursion available
        }
        ucData++;
        if ((*ucData++) || (*ucData++ != 1)) {
            fnDNS_error(DNS_ERROR_GENERAL);
            return -1;                                                   // more that one answer to a single question...
        }

        usDNS_ancount = *ucData++;
        usDNS_ancount <<= 8;
        usDNS_ancount |= *ucData++;

        usDNS_nscount = *ucData++;
        usDNS_nscount <<= 8;
        usDNS_nscount |= *ucData++;

        ucData += 2;                                                     // skip additional RRs

        cTmpPtr = ptrDNS_host_name;
        while ((ucLabLen = *ucData++) != 0) {                            // label length
            if (uMemcmp(ucData, (const unsigned char *)cTmpPtr, ucLabLen)) {
                return -1;                                               // compare with the searched host name
            }
            ucData += ucLabLen;
            cTmpPtr += ucLabLen;

            if (*cTmpPtr == '\0') {              
                continue;
            }

            if (*cTmpPtr++ != '.') {
                return (-1);                                             // DOT should be here...
            }
        }

        if (uMemcmp(ucZeroQTypeClass, (ucData - 1), sizeof(ucZeroQTypeClass))) {
            fnDNS_error(DNS_ERROR_GENERAL);
            return -1;                                                   // bad type / class
        }

        ucData += (sizeof(ucZeroQTypeClass) - 1);

        fnFindAnswer(ucData, usDNS_nscount, usDNS_ancount);              // attempt to find the answer we are looking for
    }
    return 0;
}

static void fnFindAnswer(unsigned char *ucData, unsigned short usDNS_nscount, unsigned short usDNS_ancount)
{
    unsigned short usLen;

    while ((usDNS_nscount != 0) || (usDNS_ancount != 0)) {
        while ((*ucData != 0) && ((*ucData & 0xc0) != 0xc0)) {
            ucData++;
        }
        if (*ucData != 0) {
            ucData++;                                                    // jump second offset byte
        }
        ucData++;

        if ((uMemcmp(ucTypeClass, ucData, sizeof(ucTypeClass))) == 0) {  // check type and class
                                                                         // we have found it..CLASS==INET and TYPE=A
            ucData += sizeof(ucTypeClass) + 4;                           // skip TTL

            usLen = *ucData++;                                           // read RDLENGTH
            usLen <<= 8;
            usLen |= *ucData++;

            if (usLen == IPV4_LENGTH) {
                uMemcpy(ucDNS_server_temp_ip, ucData, IPV4_LENGTH);      // copy the IP address
                if (usDNS_ancount != 0) {
                    uTaskerStopTimer(OWN_TASK);                          // stop monitoring since successful
                    fnClientListener(DNS_EVENT_SUCCESS, ucDNS_server_temp_ip);
                    ucDNS_state = DNS_STATE_READY;
                }
                else {
                    ucDNSRetries = (DNS_NUM_RETRIES+1);                  // invoke another query to the authority IP address just received
                    fnRetry();
                }
                return;
            }
            else {
                ucData += usLen;                                         // skip data
            }
        }
        else {
            ucData += (sizeof(ucTypeClass) + 4);                         // skip TTL

            usLen = *ucData++;                                           // read RDLENGTH
            usLen <<= 8;
            usLen |= *ucData++;

            ucData += usLen;                                             // skip data
        }

        if (usDNS_ancount != 0) {                                        // decrement counters
            usDNS_ancount--;
        }
        else {
            if (usDNS_nscount != 0) {
                usDNS_nscount--;
            }
        }
    }
}

static void fnDNS_error(unsigned char ucError)
{
    fnClientListener(ucError, 0);
}
#endif


#if defined USE_mDNS_SERVER
#if !defined USE_IGMP
    #error "mDNS requires IGMP to be enabled!!"
#endif

#define mDNS_CACHE_ENTRIES 2
static const CHAR *mDNS_cache[mDNS_CACHE_ENTRIES] = {                    // temporary list of local names that we resolve to
    "rndis.local.",
    "test.local.",
};

static int fnMatch_mDNS_chache(unsigned char **p_ptrBuf, unsigned short *ptr_usDataLen)
{
    int iCacheEntry = 0;
    unsigned char *ptrName = *p_ptrBuf;
  //unsigned short usLength = *ptr_usDataLen;
    unsigned char ucLen;
    unsigned char ucThisLength;
    unsigned char ucCacheLength;
    while (iCacheEntry < mDNS_CACHE_ENTRIES) {                           // for each cache entry available
        ucThisLength = 0;
        ucCacheLength = 0;
        ucLen = *ptrName++;                                              // length of phrase
        while (uMemcmp(&mDNS_cache[iCacheEntry][ucCacheLength], ptrName, ucLen) == 0) {
            ptrName += ucLen;
            ucCacheLength += ucLen;
            if (mDNS_cache[iCacheEntry][ucCacheLength] == '.') {
                ucCacheLength++;
                ucThisLength += (ucLen + 1);
            }
            else {
                break;
            }
            ucLen = *ptrName++;                                          // next length
            if (ucLen == 0) {                                            // complete match has been found
                ucThisLength++;
                *ptr_usDataLen = (*ptr_usDataLen - ucThisLength);
                *p_ptrBuf = (*p_ptrBuf + ucThisLength);
                return (iCacheEntry + 1);                                // return the reference of the matching cache entry
            }
        }
        ptrName = *p_ptrBuf;                                             // set back for next comparison
        iCacheEntry++;
    }
    return -1;                                                           // no match in the cache
}


static void fn_mDNS_respond(unsigned char *ptrBuf, int imDNS_cacheEntry, int iHostID, unsigned short usTransactionID, unsigned short usFlags, unsigned short usSourcePort)
{
    #define TYPE_A_HOST_ADDRESS        0x0001
    #define DNS_UNICAST_RESPONSE       0x8000
    #define DNS_CACHE_FLUSH            0x8000

    #define DNS_OP_CODE_MASK           0x7800
    #define DNS_OP_CODE_STANDARD_QUERY 0x0000
    #define DNS_FLAG_RESPONSE          0x8000
    #define DNS_DOMAIN_AUTHORITY       0x0400

    #define mDNS_TTL                   120

    unsigned short usClass;
    unsigned short usType = *ptrBuf++;
    usType <<= 8;
    usType |= *ptrBuf++;
    usClass = *ptrBuf++;
    usClass <<= 8;
    usClass |= *ptrBuf;

    if (usType == TYPE_A_HOST_ADDRESS) {                                 // type A
        UDP_DNS_MESSAGE UDP_Message;
        UDP_TX_PROPERTIES udp_properties;
        unsigned short usLength = 0;
        unsigned short usLenLocation;
        const CHAR *ptrCacheEntry = mDNS_cache[imDNS_cacheEntry];
      //if ((usClass & DNS_UNICAST_QUESTION) != 0) {                     // if a unicast response is requested
      //}
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(usTransactionID >> 8);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(usTransactionID);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)((DNS_UNICAST_RESPONSE | DNS_DOMAIN_AUTHORITY | DNS_OP_CODE_STANDARD_QUERY) >> 8);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)((DNS_UNICAST_RESPONSE | DNS_DOMAIN_AUTHORITY | DNS_OP_CODE_STANDARD_QUERY));
        UDP_Message.ucUDP_Message[usLength++] = 0;                       // no questions
        UDP_Message.ucUDP_Message[usLength++] = 0;
        UDP_Message.ucUDP_Message[usLength++] = 0;                       // single answer RR
        UDP_Message.ucUDP_Message[usLength++] = 1;
        UDP_Message.ucUDP_Message[usLength++] = 0;                       // no authority RRs
        UDP_Message.ucUDP_Message[usLength++] = 0;
        UDP_Message.ucUDP_Message[usLength++] = 0;                       // no additional RRs
        UDP_Message.ucUDP_Message[usLength++] = 0;
        do {
            usLenLocation = usLength++;
            while (*ptrCacheEntry != '.') {
                UDP_Message.ucUDP_Message[usLength++] = *ptrCacheEntry++;// insert phrase
            }
            UDP_Message.ucUDP_Message[usLenLocation] = (usLength - usLenLocation - 1);
            ptrCacheEntry++;
        } while (*ptrCacheEntry != 0);
        UDP_Message.ucUDP_Message[usLength++] = 0;                       // terminate name string
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(usType >> 8);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(usType);
        usClass |= DNS_CACHE_FLUSH;
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(usClass >> 8);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(usClass);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(mDNS_TTL >> 24);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(mDNS_TTL >> 16);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(mDNS_TTL >> 8);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(mDNS_TTL);
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(IPV4_LENGTH >> 8); // data length
        UDP_Message.ucUDP_Message[usLength++] = (unsigned char)(IPV4_LENGTH);
        uMemcpy(&UDP_Message.ucUDP_Message[usLength], &network[DEFAULT_NETWORK].ucOurIP[0], IPV4_LENGTH); // add our IP addresses

        udp_properties.SocketHandle = 0;
        udp_properties.ucIPAddress = (unsigned char *)ucMulticastIP;
        udp_properties.SocketHandle = (USOCKET)(defineNetwork(DEFAULT_NETWORK) | defineInterface(DEFAULT_NETWORK));
        udp_properties.ucOptions = (UDP_OPT_SEND_CS);
        udp_properties.ucTOS = TOS_MINIMISE_DELAY;
        udp_properties.ucTTL = 1;
        udp_properties.usDestinationPort = usSourcePort;
        udp_properties.usSourcePort = usSourcePort;

        fnSendUDP_multicast(iHostID, &udp_properties, (unsigned char *)&UDP_Message, (usLength + 4)); // send a multicast response
    }
}


static void fn_mDSN_reception(int iHostID, unsigned short usSourcePort, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen)
{
    if ((usRemotePort == mDNS_PORT) && (usDataLen > 12)) {               // expecting only mDNS destination port number (5353)
        // If the source port (usSourcePort) is also equal to 5353 it signifies a fully compliant multicast DNS server
        //
        int imDNS_cacheEntry;
        unsigned short usFlags;
        unsigned short usQuestions;
        unsigned short usTransactionID = *ptrBuf++;
        usTransactionID <<= 8;
        usTransactionID |= *ptrBuf++;
        usFlags = *ptrBuf++;
        usFlags <<= 8;
        usFlags |= *ptrBuf++;
        if ((usFlags & DNS_OP_CODE_MASK) != DNS_OP_CODE_STANDARD_QUERY) {
            return;                                                      // we ignore anything but standard queries
        }
        if ((usFlags & DNS_FLAG_RESPONSE) != 0) {
            return;                                                      // we ignore responses
        }
        usQuestions = *ptrBuf++;
        usQuestions <<= 8;
        usQuestions |= *ptrBuf++;
        ptrBuf += 6;                                                     // jump answer RRs, authority RRs and additional RRs
        usDataLen -= 12;
        while ((usQuestions-- != 0) && (usDataLen != 0)) {
            if ((imDNS_cacheEntry = fnMatch_mDNS_chache(&ptrBuf, &usDataLen)) >= 0) { // check whether we have the answer to a question in cache
                if (usDataLen >= 4) {                                    // sanity check of remaining length
                    fn_mDNS_respond(ptrBuf, (imDNS_cacheEntry - 1), iHostID, usTransactionID, usFlags, usSourcePort); // respond to the question
                }
            }
        }
        // We don't handle answer RRs, authority RRs or additional RRs
        //
    }
}


extern int fnStart_mDNS(USOCKET network_interface)
{
    return fnJoinMulticastGroup((unsigned char *)ucMulticastIP, (USOCKET)(defineNetwork(0) | defineInterface(0)), fn_mDSN_reception);  // join a multicast group
}
#endif
