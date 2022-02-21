/**
 * \file
 * \brief File for the calypso of the WE IoT design kit.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 * THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 * KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 * INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 * INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 * REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 * PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY
 * RIGHT RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT
 * IS USED. INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING
 * THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH
 * ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR
 * ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */

#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

/* ###########################################################################
 * Includes:
 */
#include <stdint.h>
#include "calypso.h"
/*
 * Includes.
 * ######################################################################### */

/* ###########################################################################
 * Macros & Typedefs:
 */

/* #############################
 * Constant Macros:
 */

#define ATEVENT_GENERAL_MASK (uint32_t)0x00000100
#define ATEVENT_WLAN_MASK (uint32_t)0x00001000
#define ATEVENT_SOCKET_MASK (uint32_t)0x00010000
#define ATEVENT_NETAPP_MASK (uint32_t)0x00100000
#define ATEVENT_MQTT_MASK (uint32_t)0x01000000
#define ATEVENT_FATALERROR_MASK (uint32_t)0x10000000
#define ATEVENT_MASK (uint32_t)0x11111100

#define ATEVENT_GENERAL(GENERATOR)            \
    GENERATOR(ATEventGeneral_, invalid)       \
    GENERATOR(ATEventGeneral_, eventstartup)  \
    GENERATOR(ATEventGeneral_, reset_request) \
    GENERATOR(ATEventGeneral_, error)         \
    GENERATOR(ATEventGeneral_, NumberOfValues)

#define ATEVENT_WLAN(GENERATOR)                         \
    GENERATOR(ATEventWlan_, invalid)                    \
    GENERATOR(ATEventWlan_, connect)                    \
    GENERATOR(ATEventWlan_, disconnect)                 \
    GENERATOR(ATEventWlan_, sta_added)                  \
    GENERATOR(ATEventWlan_, sta_removed)                \
    GENERATOR(ATEventWlan_, p2p_connect)                \
    GENERATOR(ATEventWlan_, p2p_disconnect)             \
    GENERATOR(ATEventWlan_, p2p_client_added)           \
    GENERATOR(ATEventWlan_, p2p_client_removed)         \
    GENERATOR(ATEventWlan_, p2p_devfound)               \
    GENERATOR(ATEventWlan_, p2p_request)                \
    GENERATOR(ATEventWlan_, p2p_connectfail)            \
    GENERATOR(ATEventWlan_, provisioning_status)        \
    GENERATOR(ATEventWlan_, provisioning_profile_added) \
    GENERATOR(ATEventWlan_, NumberOfValues)

#define ATEVENT_SOCKET(GENERATOR)           \
    GENERATOR(ATEventSocket_, invalid)      \
    GENERATOR(ATEventSocket_, tx_failed)    \
    GENERATOR(ATEventSocket_, asynch_event) \
    GENERATOR(ATEventSocket_, connect)      \
    GENERATOR(ATEventSocket_, accept)       \
    GENERATOR(ATEventSocket_, recv)         \
    GENERATOR(ATEventSocket_, recvfrom)     \
    GENERATOR(ATEventSocket_, NumberOfValues)

#define ATEVENT_NETAPP(GENERATOR)                        \
    GENERATOR(ATEventNetapp_, invalid)                   \
    GENERATOR(ATEventNetapp_, ipv4_acquired)             \
    GENERATOR(ATEventNetapp_, ipv6_acquired)             \
    GENERATOR(ATEventNetapp_, ip_collision)              \
    GENERATOR(ATEventNetapp_, dhcpv4_leased)             \
    GENERATOR(ATEventNetapp_, dhcpv4_released)           \
    GENERATOR(ATEventNetapp_, ipv4_lost)                 \
    GENERATOR(ATEventNetapp_, dhcp_ipv4_acquire_timeout) \
    GENERATOR(ATEventNetapp_, ipv6_lost)                 \
    GENERATOR(ATEventNetapp_, NumberOfValues)

#define ATEVENT_MQTT(GENERATOR)         \
    GENERATOR(ATEventMQTT_, invalid)    \
    GENERATOR(ATEventMQTT_, operation)  \
    GENERATOR(ATEventMQTT_, recv)       \
    GENERATOR(ATEventMQTT_, disconnect) \
    GENERATOR(ATEventMQTT_, NumberOfValues)

#define ATEVENT_FATALERROR(GENERATOR)           \
    GENERATOR(ATEventFatalError_, invalid)      \
    GENERATOR(ATEventFatalError_, device_abort) \
    GENERATOR(ATEventFatalError_, driver_abort) \
    GENERATOR(ATEventFatalError_, sync_loss)    \
    GENERATOR(ATEventFatalError_, no_cmd_ack)   \
    GENERATOR(ATEventFatalError_, cmd_timout)   \
    GENERATOR(ATEventFatalError_, NumberOfValues)

/*
 * Constant Macros.
 * ########################### */

/* #############################
 * Function Macros:
 */

/*
 * Function Macros.
 * ########################## */

/* #############################
 * Typedefs:
 */

typedef enum ATEvent_t
{
    ATEvent_Invalid,
    ATEvent_Startup,
    ATEvent_General = ATEVENT_GENERAL_MASK,
    ATEvent_GeneralResetRequest,
    ATEvent_GeneralError,

    ATEvent_Wlan = ATEVENT_WLAN_MASK,
    ATEvent_WlanConnect,
    ATEvent_WlanDisconnect,
    ATEvent_WlanStaAdded,
    ATEvent_WlanStaRemoved,
    ATEvent_Wlanp2pConnect,
    ATEvent_Wlanp2pDisconnect,
    ATEvent_Wlanp2pClientAdded,
    ATEvent_Wlanp2pClientRemoved,
    ATEvent_Wlanp2pDevFound,
    ATEvent_Wlanp2pRequest,
    ATEvemt_p2p_connectfail,
    ATEvent_WlanProvisioningStatus,
    ATEvent_WlanProvisioningProfileAdded,

    ATEvent_Socket = ATEVENT_SOCKET_MASK,
    ATEvent_SocketTxFailed,
    ATEvent_SocketAynchEvent,
    ATEvent_SocketTCPConnect,
    ATEvent_SocketTCPAccept,
    ATEvent_SocketRcvd,
    ATEvent_SocketRcvdFrom,

    ATEvent_Netapp = ATEVENT_NETAPP_MASK,
    ATEvent_NetappIP4Aquired,
    ATEvent_NetappIP6Aquired,
    ATEvent_NetappIPCollision,
    ATEvent_NetappDHCPv4_leased,
    ATEvent_NetappDHCPv4_released,
    ATEvent_NetappIPv4Lost,
    ATEvent_NetappDHCPIPv4AcquireTimeout,
    ATEvent_NetappIPv6Lost,

    ATEvent_MQTT = ATEVENT_MQTT_MASK,
    ATEvent_MQTTOperation,
    ATEvent_MQTTRecv,
    ATEvent_MQTTDisconnect,

    ATEvent_HTTPGet,
    ATEvent_Custom,

    ATEvent_FatalError = ATEVENT_FATALERROR_MASK,
    ATEvent_FatalErrorDeviceAbort,
    ATEvent_FatalErrorDriverAbort,
    ATEvent_FatalErrorSyncLost,
    ATEvent_FatalErrorNoCmdAck,
    ATEvent_FatalErrorCMDTimeout,
} ATEvent_t;

typedef enum ATEventGeneral_t
{
    ATEVENT_GENERAL(GENERATE_ENUM)
} ATEventGeneral_t;

typedef enum ATEventWlan_t
{
    ATEVENT_WLAN(GENERATE_ENUM)
} ATEventWlan_t;

typedef enum ATEventSocket_t
{
    ATEVENT_SOCKET(GENERATE_ENUM)
} ATEventSocket_t;

typedef enum ATEventNetapp_t
{
    ATEVENT_NETAPP(GENERATE_ENUM)
} ATEventNetapp_t;

typedef enum ATEventMQTT_t
{
    ATEVENT_MQTT(GENERATE_ENUM)
} ATEventMQTT_t;

typedef enum ATEventFatalError_t
{
    ATEVENT_FATALERROR(GENERATE_ENUM)
} ATEventFatalError_t;

typedef struct ATEvent_Startup_t
{
    char articleNr[16];
    char chipID[12];
    char MACAddress[18];
    uint8_t firmwareVersion[3];
} ATEvent_Startup_t;

typedef struct ATEvent_SocketTCPConnect_t
{
    uint16_t ServerPort;
    char ServerAddress[16];
} ATEvent_SocketTCPConnect_t;

typedef struct ATEvent_SocketTCPAccept_t
{
    uint8_t socketID;
    ATSocket_Family_t family;
    uint16_t clientPort;
    char clientAddress[16];
} ATEvent_SocketTCPAccept_t;

typedef struct ATEvent_SocketRcvd_t
{
    uint8_t socketID;
    uint16_t length;
    char data[2048];
} ATEvent_SocketRcvd_t;

typedef struct ATEvent_SocketRcvdFrom_t
{
    uint8_t socketID;
    uint8_t format;
    uint16_t length;
    char data[2048];
} ATEvent_SocketRcvdFrom_t;

typedef struct ATEvent_NetappIP4Aquired_t
{
    char address[16];
    char gateway[16];
    char DNS[16];
} ATEvent_NetappIP4Aquired_t;

/*
 * Typedefs.
 * ########################## */

/*
 * Macros & Typedefs.
 * ######################################################################## */

/* ##########################################################################
 * Declarations:
 */

/* #############################
 * Exported Variables:
 */

/*
 * Exported Variables.
 * ########################## */

/* #############################
 * Exported Functions:
 */
extern bool ATEvent_parseEventName(char **pAtCommand, ATEvent_t *pEvent);
extern bool ATEvent_parseEventArgumentValues(char **pCmdArguments, ATEvent_t event, void *pValues);

/*
 * Exported Functions.
 * ########################## */

/*
 * Declarations.
 * ######################################################################### */

#endif // EVENTS_H_INCLUDED
