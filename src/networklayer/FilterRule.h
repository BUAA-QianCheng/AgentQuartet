//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef NETWORKLAYER_FILTERRULE_H_
#define NETWORKLAYER_FILTERRULE_H_

#include <omnetpp.h>
#include "FilterRuleTable.h"
#include "transportlayer/PortRange_m.h"

#include "inet/common/INETDefs.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/transportlayer/tcp_common/TcpHeader.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/IpProtocolId_m.h"

using namespace inet;
using namespace std;
using namespace tcp;

class FilterRuleTable;

enum RuleType {
    UNSET = 0,
    IPV4_,
    IPV6_,
    IPV4_ICMP,
    IPV4_TCP,
    IPV4_UDP,
    IPV6_ICMP,
    IPV6_TCP,
    IPV6_UDP
};

enum ActionType {
    M = 0, //MATCH
    AC = 1, //ACCEPT
    D = 2, //DROP
    R = 3  //RATE_LIMIT
};

class FilterRule : public cObject{
  private:
    FilterRuleTable *frt = nullptr;
    RuleType ruleType = UNSET;
    ActionType action = M;
    double limit=0.0;
    //L3 Details
    L3Address srcNetwork, destNetwork;
    int srcNetworkPrefixLength = 0, destNetworkPrefixLength = 0;
    int l3Length=0, l3Protocol=0, fragOp=0, fragType=0;
    //L4 Details
    PortRange srcPortRange, destPortRange;
    int tcpOp=0;
    int tcpBitmask=0, udpLength=0;
    int icmpCode=-1, icmpType=-1;
    //statistic
    int pktsDropped=0, octetsDropped=0;

  public:
    //Strs
    virtual string Ipv4Str() const;
    virtual string Ipv6Str() const;
    virtual string portRangeStr(PortRange pr) const;
    virtual string TcpStr() const;
    virtual string UdpStr() const;
    virtual string IcmpStr() const;
    virtual string ruleTypeStr() const;

    virtual string str() const override;
    virtual string detailedInfo() const OMNETPP5_CODE(override);

    //sete & get
    virtual void setFilterRuleTable(FilterRuleTable *frt){ this->frt = frt; }
    FilterRuleTable *getFilterRuleTable() const { return frt; }

    virtual void setRuleType (RuleType type){ this->ruleType = type; }
    virtual RuleType getRuleType () { return ruleType; }

    virtual void setAction (ActionType actionType) { this->action = actionType; }
    virtual void setLimit(double d) { this->limit = d; }
    virtual void setSrcNetwork(L3Address srcNetwork) { this->srcNetwork = srcNetwork; }
    virtual void setDestNetwork(L3Address destNetwork) { this->destNetwork = destNetwork; }
    virtual void setSrcNetworkPrefixLength(int prefixLength) { this->srcNetworkPrefixLength = prefixLength; }
    virtual void setDestNetworkPrefixLength(int prefixLength) { this->destNetworkPrefixLength = prefixLength; }
    virtual void setL3length(int length){ this->l3Length = length; }
    virtual void setL3Protocol(int protocol){ this->l3Protocol = protocol; }
    virtual void setFragOp(int op){ this->fragOp = op; }
    virtual void setFragType(int type){ this->fragType = type; }
    virtual void setSrcPortRange(PortRange pr){ this->srcPortRange = pr; }
    virtual void setDestPortRange(PortRange pr){ this->destPortRange = pr; }
    virtual void setTcpOp(int op){ this->tcpOp = op; }
    virtual void setTcpBitMask(int bitmask){ this->tcpBitmask = bitmask; }
    virtual void setUdpLength(int length){ this->udpLength = length; }
    virtual void setIcmpCode(int code){ this->icmpCode = code; }
    virtual void setIcmpType(int type){ this->icmpType = type; }

    virtual void setPktsDropped(int num) { this->pktsDropped = num; }
    virtual int getPktsDropped() { return this->pktsDropped; }
    virtual void addPktsDropped(int num) { this->pktsDropped += num; }

    virtual void setOctetsDropped(int num) { this->octetsDropped = num; }
    virtual int getOctetDropped() { return this->octetsDropped; }
    virtual void addOctetsDropped(int num) { this->octetsDropped += num; }

    virtual ActionType getAction() { return action;}
    virtual void readAction(string action);
    virtual uint8_t readOp(string op);
    virtual void readPortRange(string portRange, PortRange *pr);
    virtual void readL3(string l3);
    virtual void readL4(string l4);
    virtual void readFilterRule(string filterRule);

    virtual bool isPortRangeUnspecified(PortRange pr) const;
    virtual bool inPortRange(uint16_t portNum, PortRange pr) const;
    virtual uint16_t getTcpFlags(Ptr<const TcpHeader> h) const;

    virtual bool matchL4(Packet *packet, IpProtocolId id) const;
    virtual bool matchPacket(Packet *packet) const;

    virtual void genFilterRuleFromPacket(Packet* packet, ActionType action);

  public:
    FilterRule() : frt(nullptr) {}
    ~FilterRule(){}
    FilterRule(string filterRule){ this->readFilterRule(filterRule); }

};

#endif /* NETWORKLAYER_FILTERRULE_H_ */
