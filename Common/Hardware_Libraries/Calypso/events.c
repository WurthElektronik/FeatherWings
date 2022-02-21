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

/* ###########################################################################
 * Includes:
 */

#include "events.h"

/*
 * Includes.
 * ######################################################################### */

/* ###########################################################################
 * Macros & Typedefs:
 */

/* #############################
 * Constant Macros:
 */

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
 * Exported Globals:
 */

/*
 * Exported Globals.
 * ########################## */

/* #############################
 * Static Globals:
 */

static const char *ATEvent_GeneralStrings[] =
    {
        ATEVENT_GENERAL(GENERATE_STRING)};

static const char *ATEvent_WLANStrings[] =
    {
        ATEVENT_WLAN(GENERATE_STRING)};

static const char *ATEvent_SocketStrings[] =
    {
        ATEVENT_SOCKET(GENERATE_STRING)};

static const char *ATEvent_NetappStrings[] =
    {
        ATEVENT_NETAPP(GENERATE_STRING)};

static const char *ATEvent_MQTTStrings[] =
    {
        ATEVENT_MQTT(GENERATE_STRING)};

static const char *ATEvent_FatalErrorStrings[] =
    {
        ATEVENT_FATALERROR(GENERATE_STRING)};

/*
 * Static Globals.
 * ########################## */

/* #############################
 * Static Functions:
 */

static bool ATEvent_parseStartUpArgumentValues(char **pCmdArguments, ATEvent_t event, void *pValues);
static bool ATEvent_parseSocketArgumentValues(char **pCmdArguments, ATEvent_t event, void *pValues);
static bool ATEvent_parseNetappArgumentValues(char **pCmdArguments, ATEvent_t event, void *pValues);

static bool ATEvent_parseEventGeneral(const char *eventGeneralString, ATEvent_t *pOutEvent);
static bool ATEvent_parseEventWlan(const char *eventWlanString, ATEvent_t *pOutEvent);
static bool ATEvent_parseEventSocket(const char *eventSocketString, ATEvent_t *pOutEvent);
static bool ATEvent_parseEventNetapp(const char *eventNetappString, ATEvent_t *pOutEvent);
static bool ATEvent_parseEventMQTT(const char *eventMQTTString, ATEvent_t *pOutEvent);
static bool ATEvent_parseEventFatalError(const char *eventFatalErrorString, ATEvent_t *pOutEvent);
/*
 * Static Functions.
 * ########################## */

/*
 * Declarations.
 * ######################################################################## */

/* ###########################################################################
 * Definitions:
 */

/* #############################
 * Exported Functions:
 */

/**@brief Parses the command and returns the respective ATEvent_t
 *
 * -   pAtCommand  AT command starting with '+'
 * @param[out]  pEvent      ATEvent_t representing the event
 *
 * return true if parsed succesful, false otherwise
 */
bool ATEvent_parseEventName(char **pAtCommand, ATEvent_t *pEvent)
{
    bool ret = false;
    char cmdName[32];
    char option[64];

    *pEvent = ATEvent_Invalid;
    ret = Calypso_getCmdName(pAtCommand, cmdName, EVENT_DELIM);
    if (ret)
    {
        if (0 == strcasecmp(cmdName, "+eventgeneral"))
        {
            ret = Calypso_getNextArgumentString(pAtCommand, option, ARGUMENT_DELIM);
            if (ret)
            {
                ATEvent_parseEventGeneral(option, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventwlan"))
        {
            ret = Calypso_getNextArgumentString(pAtCommand, option, ARGUMENT_DELIM);
            if (ret)
            {
                ATEvent_parseEventWlan(option, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventsocket"))
        {
            ret = Calypso_getNextArgumentString(pAtCommand, option, ARGUMENT_DELIM);
            if (ret)
            {
                ATEvent_parseEventSocket(option, pEvent);
            }
        }

        else if (0 == strcasecmp(cmdName, "+eventnetapp"))
        {
            ret = Calypso_getNextArgumentString(pAtCommand, option, ARGUMENT_DELIM);
            if (ret)
            {
                ATEvent_parseEventNetapp(option, pEvent);
            }
        }

        else if (0 == strcasecmp(cmdName, "+eventmqtt"))
        {
            ret = Calypso_getNextArgumentString(pAtCommand, option, ARGUMENT_DELIM);
            if (ret)
            {
                ATEvent_parseEventMQTT(option, pEvent);
            }
        }

        else if (0 == strcasecmp(cmdName, "+eventfatalerror"))
        {
            ret = Calypso_getNextArgumentString(pAtCommand, option, ARGUMENT_DELIM);
            if (ret)
            {
                ATEvent_parseEventFatalError(option, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventstartup"))
        {
            *pEvent = ATEvent_Startup;
        }
        else if (0 == strcasecmp(cmdName, "+recv"))
        {
            *pEvent = ATEvent_SocketRcvd;
        }
        else if (0 == strcasecmp(cmdName, "+recvfrom"))
        {
            *pEvent = ATEvent_SocketRcvdFrom;
        }
        else if (0 == strcasecmp(cmdName, "+connect"))
        {
            *pEvent = ATEvent_SocketTCPConnect;
        }
        else if (0 == strcasecmp(cmdName, "+accept"))
        {
            *pEvent = ATEvent_SocketTCPAccept;
        }
        else if (0 == strcasecmp(cmdName, "+eventcustom"))
        {
            *pEvent = ATEvent_Custom;
        }
        else if (0 == strcasecmp(cmdName, "+eventhttpget"))
        {
            *pEvent = ATEvent_HTTPGet;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

/**@brief Parses the values of the arguments
 *
 * -   pCmdArguments string with arguments of the AT command
 * -   event         ATEvent_t representing the event
 * @param[out]  pValues       values of the event
 *
 * return true if parsed succesful, false otherwise
 */
extern bool ATEvent_parseEventArgumentValues(char **pCmdArguments, ATEvent_t event, void *pValues)
{
    bool ret = false;
    ATEvent_t eventType = ATEVENT_MASK & event;

    switch (eventType)
    {
    case ATEvent_Startup:
    {
        ret = ATEvent_parseStartUpArgumentValues(pCmdArguments, event, pValues);
        break;
    }

    case ATEvent_Socket:
    {
        ret = ATEvent_parseSocketArgumentValues(pCmdArguments, event, pValues);
        break;
    }
    case ATEvent_Netapp:
    {
        ret = ATEvent_parseNetappArgumentValues(pCmdArguments, event, pValues);
    }
    default:
    {
        break;
    }
    }

    return ret;
}

/*
 * Exported Functions.
 * ########################## */

/* #############################
 * Static Functions:
 */

/**@brief Parses the values of the arguments
 *
 * @param pcmdArguments Pointer to string containing arguments
 *    -   event    ATEvent_t representing the event
 * @param[out]  pValues  values of the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseStartUpArgumentValues(char **pCmdArguments, ATEvent_t event, void *pValues)
{
    bool ret = false;

    ATEvent_Startup_t *startUpValues = pValues;

    ret = Calypso_getNextArgumentString(pCmdArguments, startUpValues->articleNr, ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_getNextArgumentString(pCmdArguments, startUpValues->chipID, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_getNextArgumentString(pCmdArguments, startUpValues->MACAddress, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_getNextArgumentInt(pCmdArguments, &(startUpValues->firmwareVersion[0]), (INTFLAGS_SIZE8), '.');
    }

    if (ret)
    {
        ret = Calypso_getNextArgumentInt(pCmdArguments, &(startUpValues->firmwareVersion[1]), (INTFLAGS_SIZE8), '.');
    }

    if (ret)
    {
        ret = Calypso_getNextArgumentInt(pCmdArguments, &(startUpValues->firmwareVersion[2]), (INTFLAGS_SIZE8), STRING_TERMINATE);
    }

    return ret;
}
/**@brief Parses the values of the arguments
 *
 * @param pcmdArguments Pointer to string containing arguments
 * -   event    ATEvent_t representing the event
 * @param[out]  pValues  values of the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseSocketArgumentValues(char **pCmdArguments, ATEvent_t event, void *pValues)
{
    bool ret = false;

    switch (event)
    {
    case ATEvent_SocketTCPConnect:
    {
        ATEvent_SocketTCPConnect_t *values = pValues;

        ret = Calypso_getNextArgumentInt(pCmdArguments, &(values->ServerPort), INTFLAGS_SIZE16, ARGUMENT_DELIM);

        if (ret)
        {
            ret = Calypso_getNextArgumentString(pCmdArguments, values->ServerAddress, STRING_TERMINATE);
        }

        break;
    }
    case ATEvent_SocketTCPAccept:
    {
        ATEvent_SocketTCPAccept_t *values = pValues;
        char temp[12];

        ret = Calypso_getNextArgumentInt(pCmdArguments, &(values->socketID), INTFLAGS_SIZE8, ARGUMENT_DELIM);

        if (ret)
        {
            ret = Calypso_getNextArgumentString(pCmdArguments, temp, ARGUMENT_DELIM);
            if (ret)
            {
                ret = ATSocket_parseSocketFamily(temp, &(values->family));
            }
        }

        if (ret)
        {
            ret = Calypso_getNextArgumentInt(pCmdArguments, &(values->clientPort), INTFLAGS_SIZE16, ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = Calypso_getNextArgumentString(pCmdArguments, values->clientAddress, STRING_TERMINATE);
        }

        break;
    }

    case ATEvent_SocketRcvd:
    {
        ATEvent_SocketRcvd_t *values = pValues;
        ret = Calypso_getNextArgumentInt(pCmdArguments, &(values->socketID), INTFLAGS_SIZE8, ARGUMENT_DELIM);

        if (ret)
        {
            ret = Calypso_getNextArgumentInt(pCmdArguments, &(values->length), INTFLAGS_SIZE16, ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = Calypso_getNextArgumentString(pCmdArguments, values->data, STRING_TERMINATE);
        }

        break;
    }

    case ATEvent_SocketRcvdFrom:
    {
        ATEvent_SocketRcvdFrom_t *values = pValues;
        ret = Calypso_getNextArgumentInt(pCmdArguments, &(values->socketID), INTFLAGS_SIZE8, ARGUMENT_DELIM);

        if (ret)
        {
            ret = Calypso_getNextArgumentInt(pCmdArguments, &(values->format), INTFLAGS_SIZE8, ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = Calypso_getNextArgumentInt(pCmdArguments, &(values->length), INTFLAGS_SIZE16, ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = Calypso_getNextArgumentString(pCmdArguments, values->data, STRING_TERMINATE);
        }

        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}
/**@brief Parses the values of the arguments
 *
 * -   event    ATEvent_t representing the event
 * @param[out]  pValues  values of the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseNetappArgumentValues(char **pCmdArguments, ATEvent_t event, void *pValues)
{
    bool ret = false;
    switch (event)
    {
    case ATEvent_NetappIP4Aquired:
    {
        ATEvent_NetappIP4Aquired_t *values = pValues;
        ret = Calypso_getNextArgumentString(pCmdArguments, values->address, ARGUMENT_DELIM);

        if (ret)
        {
            ret = Calypso_getNextArgumentString(pCmdArguments, values->gateway, ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = Calypso_getNextArgumentString(pCmdArguments, values->DNS, STRING_TERMINATE);
        }
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

/**@brief Parses a string to ATEvent_t
 *
 * -   eventGeneralString   string representing event option
 * @param[out]  pOutEvent            ATEvent_t representing the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseEventGeneral(const char *eventGeneralString, ATEvent_t *pOutEvent)
{
    for (int i = 0; i < ATEventGeneral_NumberOfValues; i++)
    {
        if (0 == strcasecmp(eventGeneralString, ATEvent_GeneralStrings[i]))
        {
            *pOutEvent = ATEvent_General + (ATEvent_t)i;
            return true;
        }
    }

    return false;
}

/**@brief Parses a string to ATEvent_t
 *
 * -   eventWlanString      string representing event option
 * @param[out]  pOutEvent            ATEvent_t representing the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseEventWlan(const char *eventWlanString, ATEvent_t *pOutEvent)
{
    for (int i = 0; i < ATEventWlan_NumberOfValues; i++)
    {
        if (0 == strcasecmp(eventWlanString, ATEvent_WLANStrings[i]))
        {
            *pOutEvent = ATEvent_Wlan + (ATEvent_t)i;
            return true;
        }
    }

    return false;
}

/**@brief Parses a string to ATEvent_t
 *
 * -   eventWlanString      string representing event option
 * @param[out]  pOutEvent            ATEvent_t representing the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseEventSocket(const char *eventSocketString, ATEvent_t *pOutEvent)
{
    for (int i = 0; i < ATEventSocket_NumberOfValues; i++)
    {
        if (0 == strcasecmp(eventSocketString, ATEvent_SocketStrings[i]))
        {
            *pOutEvent = ATEvent_Socket + (ATEvent_t)i;
            return true;
        }
    }

    return false;
}

/**@brief Parses a string to ATEvent_t
 *
 * -   eventNetappString      string representing event option
 * @param[out]  pOutEvent            ATEvent_t representing the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseEventNetapp(const char *eventNetappString, ATEvent_t *pOutEvent)
{
    for (int i = 0; i < ATEventNetapp_NumberOfValues; i++)
    {
        if (0 == strcasecmp(eventNetappString, ATEvent_NetappStrings[i]))
        {
            *pOutEvent = ATEvent_Netapp + (ATEvent_t)i;
            return true;
        }
    }

    return false;
}

/**@brief Parses a string to ATEvent_t
 *
 * -   eventMQTTString      string representing event option
 * @param[out]  pOutEvent            ATEvent_t representing the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseEventMQTT(const char *eventMQTTString, ATEvent_t *pOutEvent)
{
    for (int i = 0; i < ATEventMQTT_NumberOfValues; i++)
    {
        if (0 == strcasecmp(eventMQTTString, ATEvent_MQTTStrings[i]))
        {
            *pOutEvent = ATEvent_MQTT + (ATEvent_t)i;
            return true;
        }
    }

    return false;
}

/**@brief Parses a string to ATEvent_t
 *
 * -   eventFatalErrorString      string representing event option
 * @param[out]  pOutEvent            ATEvent_t representing the event
 *
 * return true if parsed succesful, false otherwise
 */
static bool ATEvent_parseEventFatalError(const char *eventFatalErrorString, ATEvent_t *pOutEvent)
{
    for (int i = 0; i < ATEventFatalError_NumberOfValues; i++)
    {
        if (0 == strcasecmp(eventFatalErrorString, ATEvent_FatalErrorStrings[i]))
        {
            *pOutEvent = ATEvent_FatalError + (ATEvent_t)i;
            return true;
        }
    }

    return false;
}

/*
 * Static Functions.
 * ########################## */

/*
 * Definitions.
 * ######################################################################### */
