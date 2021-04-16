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

#include "FilterRule.h"

#include <inet/common/Protocol.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>
#include <inet/networklayer/ipv6/Ipv6Header.h>
#include <inet/networklayer/contract/ipv4/Ipv4Address.h>
#include <inet/networklayer/contract/ipv6/Ipv6Address.h>
#include <inet/networklayer/ipv4/IcmpHeader_m.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/tcp_common/TcpHeader_m.h>
#include <inet/transportlayer/udp/UdpHeader_m.h>
#include <inet/transportlayer/contract/TransportHeaderBase_m.h>

using namespace inet;

Register_Class(FilterRule);

string FilterRule::Ipv4Str() const{
    stringstream out;
    if(srcNetwork.toIpv4().isUnspecified())
        out << "* > ";
    else
        out << srcNetwork.toIpv4().str() << "/" << srcNetworkPrefixLength << " > ";
    if(destNetwork.toIpv4().isUnspecified())
        out << "*";
    else
        out << destNetwork.toIpv4().str() << "/" << destNetworkPrefixLength;
    if(l3Length == 0 && l3Protocol == 0 && fragOp == 0){
        out << "; ";
    }
    else {
        out <<" (";
        int num=0;
        if(l3Length != 0){
            out << "Length=" << l3Length;
            num++;
        }
        if(l3Protocol != 0){
            out << (num == 0 ? "" : ", ") << "Protocol=" << l3Protocol;
            num++;
        }
        if(fragOp != 0){
            out << (num == 0 ? "" : ", ");
            out << (fragOp & (1<<3)? "NOT" : "") << (fragOp & (1<<1) ? " MATCH" : "") << (fragOp & 1 ? " ANY": "") << fragType;
        }
        out << "); ";
    }
    return out.str();
}

string FilterRule::Ipv6Str() const {
    stringstream out;
    if(srcNetwork.toIpv6().isUnspecified())
        out << "* > ";
    else
        out << srcNetwork.toIpv6().str() << "/" << srcNetworkPrefixLength << " > ";
    if(destNetwork.toIpv6().isUnspecified())
        out << "*";
    else
        out << destNetwork.toIpv6().str() << "/" << destNetworkPrefixLength;
    if(l3Length == 0 && l3Protocol == 0 && fragOp == 0){
            out << "; ";
    }
    else {
        out <<" ( ";
        int num=0;
        if(l3Length != 0){
            out << "Length=" << l3Length;
            num++;
        }
        if(l3Protocol != 0){
            out << (num == 0 ? "" : ", ") << "Protocol=" << l3Protocol;
            num++;
        }
        if(fragOp != 0 ){
            out << (num == 0 ? "" : ", ");
            out << (fragOp & (1<<3)? "NOT " : "") << (fragOp & (1<<1) ? "MATCH " : "") << (fragOp & 1 ? "ANY ": "") << fragType;
        }
        out << "); ";
    }
    return out.str();
}

string FilterRule::portRangeStr(PortRange pr) const{
    std::stringstream out;
    if(pr.lowerPort == pr.upperPort && pr.op == NO_OP){
        out << "*" ;
        return out.str();
    }
    switch(pr.op){
    case LTE:
        out << "<=" << pr.lowerPort;
        break;
    case GTE:
        out << ">=" << pr.lowerPort;
        break;
    case EQ:
        out << pr.lowerPort;
        break;
    case NEQ:
        out << "!=" << pr.lowerPort;
        break;
    default: //NO_OP
        out << pr.lowerPort <<"-"<< pr.upperPort;
        break;
    }
    return out.str();
}

string FilterRule::TcpStr() const{
    stringstream out;
    out << portRangeStr(srcPortRange) << " > " << portRangeStr(destPortRange);
    if(tcpOp != 0){
        out <<" (" <<(tcpOp & (1<<3)? "NOT " : "") << (tcpOp & (1<<1) ? "MATCH " : "") << (tcpOp & 1 ? "ANY ": "");
        out << tcpBitmask << ")";
    }
    return out.str();
}

string FilterRule::UdpStr() const{
    stringstream out;
    out << portRangeStr(srcPortRange) << " > " << portRangeStr(destPortRange);
    if(udpLength != 0){
        out << " (Length=" << udpLength<< ")";
    }
    return out.str();
}

string FilterRule::IcmpStr() const{
    stringstream out;
    if(icmpType == -1 && icmpCode == -1){
        return out.str();
    }
    else {
        out << " (" ;
        if(icmpType != -1 && icmpCode != -1)
            out << "Code=" << icmpCode << ", Type=" << icmpType;
        else{
            if(icmpType != -1)
                out << "Type=" << icmpType;
            else
                out << "Code=" << icmpCode;
        }
        out << ")";
    }
    return out.str();
}

string FilterRule::ruleTypeStr() const {
    switch(ruleType){
    case IPV4_:
        return "IPV4_";
    case IPV6_:
        return "IPV6_";
    case IPV4_ICMP:
        return "IPV4_ICMP";
    case IPV4_TCP:
        return "IPV4_TCP";
    case IPV4_UDP:
        return "IPV4_UDP";
    case IPV6_ICMP:
        return "IPV6_ICMP";
    case IPV6_TCP:
        return "IPV6_TCP";
    case IPV6_UDP:
        return "IPV6_UDP";
    default:
        return "UNSET";
    }
}


string FilterRule::str() const {
    stringstream out;
    //Action
    switch(action){
    case M:
        out << "MATCH; ";
        break;
    case AC:
        out << "ACCEPT; ";
        break;
    case D:
        out << "DROP; ";
        break;
    case R:
        out << "RATE_LIMIT " << limit <<"; ";
        break;
    }
    //Rule Details
    switch(ruleType){
    case IPV4_:
        out << Ipv4Str();
        out << portRangeStr(srcPortRange) << " > " << portRangeStr(destPortRange);
        break;
    case IPV6_:
        out << Ipv6Str();
        out << portRangeStr(srcPortRange) << " > " << portRangeStr(destPortRange);
        break;
    case IPV4_ICMP:
        out << Ipv4Str();
        out << IcmpStr();
        break;
    case IPV4_TCP:
        out << Ipv4Str();
        out << TcpStr();
        break;
    case IPV4_UDP:
        out << Ipv4Str();
        out << UdpStr();
        break;
    case IPV6_ICMP:
        out << Ipv4Str();
        out << IcmpStr();
        break;
    case IPV6_TCP:
        out << Ipv6Str();
        out << TcpStr();
        break;
    case IPV6_UDP:
        out << Ipv6Str();
        out << UdpStr();
        break;
    default:
        out << "* > *; * > *";
        break;
    }
    return out.str();
}

string FilterRule::detailedInfo() const {
    return str();
}

void FilterRule::readAction(string action){
    const char * s = action.c_str();
    if(strncmp(s,"ACCEPT", 6) == 0){
        this->action = AC;
    }
    else if(strncmp(s,"DROP", 4) == 0){
        this->action = D;
    }
    else if(strncmp(s,"MATCH", 5) == 0){
        this->action = M;
    }
    else if(strncmp(s,"RATE_LIMIT", 10) == 0){
        this->action = R;
        limit = 0.0;
        for(int i = 10, dot = 0; i<strlen(s); i++){
            if(s[i] >= '0' && s[i] <= '9'){
                if(dot == 0)
                    limit = limit*10.0 + (s[i]-'0');
                else
                    limit = limit + pow(0.1, dot++) * (s[i]-'0');
            }
            else if(s[i] == '.')
                dot = 1;
            else{
                throw cRuntimeError("readAction(): Error in RATE_LIMIT! Check your Rule! %s", action.c_str());
                return ;
            }
        }
    }
    else{
        throw cRuntimeError("readAction(): Wrong ACTION! Check your Rule! %s", action.c_str());
        return ;
    }
}

uint8_t FilterRule::readOp(string op){
    //enum OpValue{    3    2    1    0
    // ANY = 1; }      8    4    2    1
    // MATCH = 2;     NOT      MATCH ANY
    // NOT_ANY = 9;
    // NOT_MATCH = 10;
    //};
    if(strncmp(op.c_str(),"ANY",3) == 0){
        return 1;
    }
    if(strncmp(op.c_str(),"MATCH",5) == 0){
        return 2;
    }
    if(strncmp(op.c_str(),"NOTANY",6) == 0){
        return 9;
    }
    if(strncmp(op.c_str(),"NOTMATCH",8) == 0){
        return 10;
    }
    return 0;
}
void FilterRule::readPortRange(string portRange, PortRange *pr){
    int portNum = 0, i;
    if(portRange[0]>='0' && portRange[0]<='9'){//begin with digit
        for(i = 0; portRange[i]>='0' && portRange[i]<='9' && i<portRange.length(); i++){
            portNum=portNum*10 + (portRange[i] - '0');
        }
        pr->lowerPort = portNum;
        if(portRange[i] == '-'){
            portNum=0;
            for(i+=1 ; portRange[i]>='0' && portRange[i]<='9' && i<portRange.length();i++){
                portNum=portNum*10 + (portRange[i] - '0');
            }
            pr->upperPort = portNum;
            if(pr->lowerPort>=pr->upperPort){
                throw cRuntimeError("parseExpression() : lowerPort is larger than upperPort");
                return ;
            }
        }
        else{
            pr->op=EQ;
            return ;
        }
    }
    else if(portRange[0] == '>' || portRange[0] == '<' || portRange[0] == '!'){//begin with op
        pr->upperPort=-1;
        switch(portRange[0]){
        case '>':
            pr->op=GTE;
            break;
        case '<':
            pr->op=LTE;
            break;
        case '!':
            pr->op=NEQ;
            break;
        default:
            throw cRuntimeError("readPortRange() : wrong op in srcPortRange");
            return ;
        }
        for(i=2 ; portRange[i]>='0' && portRange[i]<='9' && i<portRange.length();i++){
            portNum=portNum*10 + (portRange[i] - '0');
        }
        pr->lowerPort = portNum;
    }
    else{
        throw cRuntimeError("readPortRange() : wrong op in srcPortRange");
        return ;
    }
}

void FilterRule::readL3(string l3){
    int mark = 0;
    string srcNetworkStr, destNetworkStr;
    //check '>'
    if(l3.find('>') == l3.npos){
        throw cRuntimeError("readL3(): Lack '>'! Check your rule! %s", l3.c_str());
        return ;
    }
    srcNetworkStr = std::string(l3, mark, l3.find('>'));

    if(srcNetworkStr.length() == 1 && srcNetworkStr[0] == '*'){
        //ANY srcNetwork
    }
    else{
        if(srcNetworkStr.find('/') == srcNetworkStr.npos){
            throw cRuntimeError("readL3(): srcNetwork Lack '/'! Check your rule! %s", l3.c_str());
            return ;
        }
        std::string addrStr = std::string(srcNetworkStr, 0 , srcNetworkStr.find('/'));
        std::string prefixStr = std::string(srcNetworkStr, srcNetworkStr.find('/')+1, srcNetworkStr.length()-srcNetworkStr.find('/')+1);
        L3AddressResolver().tryResolve(addrStr.c_str() ,srcNetwork);
        srcNetworkPrefixLength = 0;
        for(int i=0; i<prefixStr.length(); i++){
            if(prefixStr[i] >='0' && prefixStr[i]<='9')
                srcNetworkPrefixLength = srcNetworkPrefixLength*10 +(prefixStr[i]-'0');
            else
                throw cRuntimeError("readL3(): Wrong Character in srcNetwork! Check Your Rule! %s, l3.c_str()");
        }
        if(srcNetworkPrefixLength > 32 || srcNetworkPrefixLength < 0){
            throw cRuntimeError("readL3(): Wrong Prefix Length in srcNetwork! Check Your Rule! %s, l3.c_str()");
        }
        this->ruleType = (srcNetwork.getType() == 1 ? IPV4_ : IPV6_);
    }
    mark = l3.find('>');
    //then destNetwork
    //locate end of addr
    if(l3.find('(') == l3.npos){
        //no Condition
        destNetworkStr = std::string(l3, mark+1, l3.length() -1);
    }
    else{
        destNetworkStr = std::string(l3, mark+1, l3.find('(', mark+1) - mark - 1);
    }
    //parse Addr
    if(destNetworkStr.length() == 1 && srcNetworkStr[0] == '*'){
        //ANY destNetwork
    }
    else{
        if(destNetworkStr.find('/') == destNetworkStr.npos){
            throw cRuntimeError("readL3(): destNetwork Lack '/'! Check your rule! %s", l3.c_str());
            return ;
        }
        std::string addrStr = std::string(destNetworkStr,0,destNetworkStr.find('/'));
        std::string prefixStr = std::string(destNetworkStr, destNetworkStr.find('/')+1, destNetworkStr.length()-destNetworkStr.find('/')+1);
        L3AddressResolver().tryResolve(addrStr.c_str() ,destNetwork);
        destNetworkPrefixLength = 0;
        for(int i=0;i<prefixStr.length();i++){
            if(prefixStr[i] >='0' && prefixStr[i]<='9')
                destNetworkPrefixLength = destNetworkPrefixLength*10 +(prefixStr[i]-'0');
            else
                throw cRuntimeError("readL3(): Wrong Character in destNetwork! Check Your Rule! %s, l3.c_str()");
        }
        if(destNetworkPrefixLength > 32 || destNetworkPrefixLength < 0){
            throw cRuntimeError("readL3(): Wrong Prefix Length in destNetwork! Check Your Rule! %s, l3.c_str()");
        }
        this->ruleType = (destNetwork.getType() == 1 ? IPV4_ : IPV6_);
    }
    mark = l3.find('(');
    if(mark != l3.npos && l3.find(')') != mark+1){
        l3Length = 0, l3Protocol = 0;
        do{
            std::string tmp;
            if(l3.find(',', mark+1) == l3.npos) //no ',' only one condition
                tmp = std::string(l3, mark+1, l3.find(')', mark+1) - mark - 1);
            else
                tmp = std::string(l3, mark+1, l3.find(',', mark+1) - mark - 1);
            if(strncmp(tmp.c_str(), "Length=", 7) == 0){
                for(int i = 7; i<tmp.length(); i++){
                    if(tmp[i] >= '0' && tmp[i] <= '9')
                        l3Length = l3Length*10 + (tmp[i]-'0');
                    else{
                        throw cRuntimeError("readL3(): Wrong L3 Condition Length! Check your Rule! %s", tmp.c_str());
                        return ;
                    }
                }
            }
            else if(strncmp(tmp.c_str(), "Protocol=", 9) == 0){
                for(int i = 9; i<tmp.length(); i++){
                    if(tmp[i] >= '0' && tmp[i] <= '9')
                        l3Protocol = l3Protocol*10 + (tmp[i]-'0');
                    else{
                        throw cRuntimeError("readL3(): Wrong L3 Condition Protocol! Check your Rule! %s", tmp.c_str());
                        return ;
                    }
                }
            }
            else if((fragOp = readOp(tmp))!= 0){
                fragType = 0;
                int j,start;
                switch(fragOp){
                case 1:
                    start=3;//NOT
                    break;
                case 2:
                    start=5;//MATCH
                    break;
                case 9:
                    start=6;//NOTANY
                    break;
                case 10:
                    start=8;//NOTMATCH
                    break;
                default:
                    start=tmp.length();//last letter
                    throw cRuntimeError("readL3(): Wrong L3 Condition Op! Check your Rule! %s", tmp.c_str());
                    return ;
                }
                for(j = start; j<tmp.length(); j++){
                    if(tmp[j] >= '0' && tmp[j] <= '9')
                        fragType = fragType*10 + (tmp[j]-'0');
                    else{
                        throw cRuntimeError("readL3(): Wrong L3 Condition fragment! Check your Rule %s", tmp.c_str());
                        return ;
                    }
                }
            }
            else{
                throw cRuntimeError("readL3(): Wrong L3 Condition! Check your Rule! %s", tmp.c_str());
                return ;
            }
            mark = l3.find(',', mark+1);
        }while(mark != l3.npos);
    }
    //Nothing will change if no condition
}

void FilterRule::readL4(string l4){
    int mark = 0;
    if(l4[0] == '*'){
        //ANY srcPort
        mark = l4.find('>');
    }
    else if(l4[0] == '('){
        ruleType = (ruleType == IPV6_ ? IPV6_ICMP : IPV4_ICMP);
        do{
            std::string tmp;
            if(l4.find(',', mark+1) == l4.npos) //no ',' only one condition
                tmp = std::string(l4, mark+1, l4.find(')', mark+1) - mark - 1);
            else
                tmp = std::string(l4, mark+1, l4.find(',', mark+1) - mark - 1);
            if(strncmp(tmp.c_str(), "Type=", 5) == 0){
                icmpType = 0;
                for(int j = 5; j<tmp.length(); j++){
                    if(tmp[j]>= '0' && tmp[j] <= '9')
                        icmpType = icmpType*10 + tmp[j]-'0';
                }
            }
            else if(strncmp(tmp.c_str(), "Code=", 5) == 0){
                icmpCode = 0;
                for(int j = 5; j<tmp.length(); j++){
                    if(tmp[j]>= '0' && tmp[j] <= '9')
                        icmpCode = icmpCode*10 + tmp[j]-'0';
                }
            }
            mark = l4.find(',', mark+1);
        }while(mark != l4.npos);
        return ;
    }
    else{//srcPortRange
        readPortRange(std::string(l4, mark, l4.find('>')), &srcPortRange);
        mark = l4.find('>');
    }

    //destPortRange
    if(l4[mark+1] == '*'){
        //ANY destPort
    }
    else{
        if(l4.find('(') == l4.npos){
            //no Condition
            readPortRange(std::string(l4, mark+1, l4.length() - mark -1), &destPortRange);
        }
        else{
            readPortRange(std::string(l4, mark+1, l4.find('(') - mark - 1), &destPortRange);
            mark = l4.find('(');
        }
    }
    if(l4.find('(') != l4.npos && l4.find(')') != mark+1){
        do{
            std::string tmp;
            if(l4.find(',', mark+1) == l4.npos) //no ',' only one condition
                tmp = std::string(l4, mark+1, l4.find(')', mark+1) - mark - 1);
            else
                tmp = std::string(l4, mark+1, l4.find(',', mark+1) - mark - 1);
            if(strncmp(tmp.c_str(), "Length=", 7) == 0){
                ruleType = (ruleType == IPV6_ ? IPV6_UDP : IPV4_UDP);
                udpLength = 0;
                for(int i = 7; i<tmp.length(); i++){
                    if(tmp[i] >= '0' && tmp[i] <= '9')
                        udpLength = udpLength*10 + (tmp[i]-'0');
                    else{
                        throw cRuntimeError("readL3(): Wrong L3 Condition Length! Check your Rule! %s", tmp.c_str());
                        return ;
                    }
                }
            }
            else if((tcpOp = readOp(tmp)) != 0){
                //this->l4 = TCP;
                ruleType = (ruleType == IPV6_ ? IPV6_TCP : IPV4_TCP);
                int j,start;
                switch(tcpOp){
                case 1:
                    start=3;//NOT
                    break;
                case 2:
                    start=5;//MATCH
                    break;
                case 9:
                    start=6;//NOTANY
                    break;
                case 10:
                    start=8;//NOTMATCH
                    break;
                default:
                    start=tmp.length();//last letter
                    throw cRuntimeError("readL4(): Wrong L4 Condition Op! Check your Rule! %s", tmp.c_str());
                    return ;
                }
                tcpBitmask=0;
                for(j = start; j<tmp.length(); j++){
                    if(tmp[j] >= '0' && tmp[j] <= '9')
                        tcpBitmask = tcpBitmask*10 + (tmp[j]-'0');
                    else{
                        throw cRuntimeError("readL4(): Wrong L4 Condition bitmask! Check your Rule! %s %d", tmp.c_str(), j);
                        return ;
                    }
                }
            }
            else{
                throw cRuntimeError("readL4(): Wrong L4 Condition! Check your Rule! %s", tmp.c_str());
            }
            mark = l4.find(',', mark+1);
        }while(mark != l4.npos);
    }
}

void FilterRule::readFilterRule(string filterRule){
    int mark = 0;
    std::string::iterator it;
    for(it = filterRule.begin(); it < filterRule.end() ; it++)//remove all spaces
        if(*it == ' ')
            filterRule.erase(it--);

    if(filterRule.length()<12){
        throw cRuntimeError("readFilterRule(): Length Less Then Min Length! Check your Rule!");
        return ;
    }
    std::string tmp;
    //Action
    if(filterRule.find(';', mark+1) == filterRule.npos)
        throw cRuntimeError("readFilterRule() : Lack ';' after Action!");
    readAction(std::string(filterRule, mark, filterRule.find(';', mark)));
    mark = filterRule.find(';' ,mark + 1);
    //l3
    if(filterRule.find(';', mark+1) == filterRule.npos)
        throw cRuntimeError("readFilterRule() : Lack ';' after L3!");
    readL3(std::string(filterRule, mark+1, filterRule.find(';',mark+1) - mark -1 ));
    mark = filterRule.find(';', mark + 1);
    //l4
    readL4(std::string(filterRule, mark+1, filterRule.length() - mark -1 ));
    EV_INFO << "FilterRule read: " << this->str() << "  FilterType: " << ruleTypeStr() << std::endl;
}

bool FilterRule::isPortRangeUnspecified(PortRange pr) const{
    if( pr.op == NO_OP && pr.lowerPort == 0 && pr.upperPort == 0)
        return true;
    return false;
}

bool FilterRule::inPortRange(uint16_t portNum, PortRange pr) const{
    if(isPortRangeUnspecified(pr))
        return true;
    switch(pr.op){
    case NO_OP:
        return (portNum >= pr.lowerPort && portNum <= pr.upperPort) ? true : false;
    case LTE:
        return (portNum <= pr.lowerPort) ? true : false;
    case GTE:
        return (portNum <= pr.lowerPort) ? true : false;
    case EQ:
        return (portNum == pr.lowerPort) ? true : false;
    case NEQ:
        return (portNum != pr.lowerPort) ? true : false;
    default:
        return false;
    }
}

uint16_t FilterRule::getTcpFlags(Ptr<const TcpHeader> h) const{
    uint16_t flags = 0;
    if (h->getCwrBit())
        flags += (1<<7);
    if (h->getEceBit())
        flags += (1<<6);
    if (h->getUrgBit())
            flags += (1<<5);
    if (h->getSynBit())
        flags += (1<<4);
    if (h->getAckBit())
        flags += (1<<3);
    if (h->getPshBit())
        flags += (1<<2);
    if (h->getRstBit())
        flags += (1<<1);
    if (h->getFinBit())
        flags += 1;
    return flags;
}

bool FilterRule::matchL4(Packet *packet, IpProtocolId id) const{
    ASSERT(packet);
    if(id == IP_PROT_ICMP && (ruleType == IPV4_ICMP || ruleType == IPV4_)){
        const auto& icmpHeader = packet->peekAtFront<IcmpHeader>();
        if(icmpCode != icmpHeader->getCode() && icmpCode != -1)
            return false ;
        if(icmpType != icmpHeader->getType() && icmpType != -1)
            return false;
        return true;
    }
    else if(id == IP_PROT_TCP && (ruleType == IPV4_TCP || ruleType == IPV4_ || ruleType == IPV6_TCP || ruleType == IPV6_)){
        const auto& tcpHeader = packet->peekAtFront<TcpHeader>();
        if(!inPortRange(tcpHeader->getSrcPort(), srcPortRange) && !isPortRangeUnspecified(srcPortRange))
            return false;
        if(!inPortRange(tcpHeader->getDestPort(), destPortRange) && !isPortRangeUnspecified(destPortRange))
            return false;
        //enum OpValue{    3    2    1    0
        // ANY = 1; }      8    4    2    1
        // MATCH = 2;     NOT      MATCH ANY
        // NOT_ANY = 9;
        // NOT_MATCH = 10;
        //};
        uint16_t flags = getTcpFlags(tcpHeader);
        switch(tcpOp){
        case 0://no tcp Condition
            if(tcpBitmask == 0)
                break;
            else{
                throw cRuntimeError("FilterRule(): Wrong tcpBitmask!");
            }
        case 1://ANY
            if((flags & tcpBitmask) == 0)
                return false;
            break;
        case 2://MATCH
            if(flags != tcpBitmask)
                return false;
            break;
        case 9://NOT ANY
            if((flags & tcpBitmask) != 0)
                return false;
            break;
        case 10:
            if(flags == tcpBitmask)
                return false;
            break;
        default:
            throw cRuntimeError("FilterRule(): Wrong tcpOp Type!");
        }
        return true;
    }
    else if(id == IP_PROT_UDP && (ruleType == IPV4_UDP || ruleType == IPV4_ || ruleType == IPV6_UDP || ruleType == IPV6_)){
        const auto& udpHeader = packet->peekAtFront<UdpHeader>();
        if(!inPortRange(udpHeader->getSrcPort(), srcPortRange) && !isPortRangeUnspecified(srcPortRange))
            return false;
        if(!inPortRange(udpHeader->getDestPort(), destPortRange) && !isPortRangeUnspecified(destPortRange))
            return false;
        if(udpLength != 0 && udpHeader->getTotalLengthField() != B(udpLength))
            return false;
        return true;
    }

    /*else if(ruleType == IPV4_ or ruleType == IPV6_){
        const auto& transportHeader = packet->peekAtFront<TransportHeaderBase>();
        if(!inPortRange(transportHeader->getSourcePort(), srcPortRange))
            return false;
        if(!inPortRange(transportHeader->getDestinationPort(), destPortRange))
            return false;
        return true;
    }*/
    else if(ruleType == UNSET || ruleType == IPV4_ || ruleType == IPV6_){
        return true;
    }
    else
        return false;
}

bool FilterRule::matchPacket(Packet *packet) const{
    ASSERT(packet);
    auto packetProtocolTag = packet->findTag<PacketProtocolTag>();
    auto protocol = packetProtocolTag->getProtocol();
    //l3 match
    if(protocol == &protocol->ipv4){
        //ipv4 match
        const auto& ipv4Header = packet->popAtFront<Ipv4Header>();
        Ipv4Address srcAddr = ipv4Header->getSrcAddress();
        Ipv4Address destAddr = ipv4Header->getDestAddress();
        if(!srcNetwork.isUnspecified() && srcAddr.getNumMatchingPrefixBits(srcNetwork.toIpv4()) < srcNetworkPrefixLength)
            return false;
        if(!destNetwork.isUnspecified() && destAddr.getNumMatchingPrefixBits(destNetwork.toIpv4()) < destNetworkPrefixLength)
            return false;
        if(packet->getBitLength() != l3Length && l3Length != 0 )
            return false;
        if(ipv4Header->getProtocolId() != l3Protocol && l3Protocol != 0)
            return false;
        //l4 match
        return matchL4(check_and_cast<Packet *>(packet), ipv4Header->getProtocolId());
    }
    else if (protocol == &protocol->ipv6){
        const auto& ipv6Header = packet->popAtFront<Ipv6Header>();
        Ipv6Address srcAddr = ipv6Header->getSrcAddress();
        Ipv6Address destAddr = ipv6Header->getDestAddress();
        if(!srcNetwork.isUnspecified() && srcAddr.matches(srcNetwork.toIpv6(), srcNetworkPrefixLength))
            return false;
        if(!destNetwork.isUnspecified() && destAddr.matches(destNetwork.toIpv6(), destNetworkPrefixLength))
            return false;
        if(packet->getBitLength() != l3Length && l3Length != 0 )
            return false;
        if(ipv6Header->getProtocolId() != l3Protocol && l3Protocol != 0)
            return false;
        //l4 match
        return matchL4(check_and_cast<Packet *>(packet), ipv6Header->getProtocolId());
    }
    return true;
}

void FilterRule::genFilterRuleFromPacket(Packet* packet, ActionType action){
    this->action = action;
    Packet *p = packet->dup();
    this->ruleType = IPV4_;
    const auto& ipv4Header = p->popAtFront<Ipv4Header>();
    L3AddressResolver().tryResolve((ipv4Header->getSrcAddress()).str(0).c_str(), srcNetwork);
    this->srcNetworkPrefixLength = 32;
    L3AddressResolver().tryResolve((ipv4Header->getDestAddress()).str(0).c_str(), destNetwork);
    this->destNetworkPrefixLength = 32;
    /*rule->setL3length(ipv4Header->getTotalLengthField());
    rule->setL3Protocol(ipv4Header->getProtocolId());
    rule->setFragOp(0);
    rule->setFragType(0);*/
    switch(ipv4Header->getProtocolId()){
    case IP_PROT_ICMP:{
        this->ruleType = IPV4_ICMP;
        const auto& icmpHeader = p->peekAtFront<IcmpHeader>();
        this->icmpType = icmpHeader->getType();
        this->icmpCode = icmpHeader->getCode();
        break;
    }
    case IP_PROT_TCP:{
        this->ruleType = IPV4_TCP;
        const auto& tcpHeader = p->peekAtFront<TcpHeader>();
        readPortRange(std::to_string(tcpHeader->getSrcPort()), &srcPortRange);
        readPortRange(std::to_string(tcpHeader->getDestPort()), &destPortRange);
        this->tcpOp = 2;//MATCH
        this->tcpBitmask = getTcpFlags(tcpHeader);
        break;
    }
    case IP_PROT_UDP:{
        this->ruleType = IPV4_UDP;
        const auto& udpHeader = p->peekAtFront<UdpHeader>();
        readPortRange(std::to_string(udpHeader->getSrcPort()), &srcPortRange);
        readPortRange(std::to_string(udpHeader->getDestPort()), &destPortRange);
        //rule->setUdpLength(udpHeader->getTotalLengthField());
        break;
    }
    default:
        break;
    }
    delete p;
}
