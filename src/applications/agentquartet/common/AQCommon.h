/*
 * AQcommon.h
 *
 *  Created on: 2021Äê3ÔÂ16ÈÕ
 *      Author: QianCheng
 */

#ifndef APPLICATIONS_AGENTQUARTET_COMMON_AQCOMMON_H_
#define APPLICATIONS_AGENTQUARTET_COMMON_AQCOMMON_H_

#include "AQClient_m.h"
#include "AQConfig_m.h"
#include "AQFilterRules_m.h"
#include "AQHeartbeat_m.h"
#include "Encryption.h"
#include "AQMitigation_m.h"
#include "AQMsgs_m.h"

enum AgentType { //ASCII:'A' = 81, 'C' = 83, 'R' = 98, 'S' = 99
    UNKNOWN= 0 ,
    CLIENT_AGENT = 8381,
    ROUTER_AGENT = 9881,
    SERVER_AGENT = 9981
};

#endif /* APPLICATIONS_AGENTQUARTET_COMMON_AQCOMMON_H_ */
