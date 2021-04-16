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

#include "ClientAgentStatus.h"

#include <string.h>

using namespace inet;

Register_Class(ClientAgentStatus);

std::string ClientAgentStatus::str() const{
    std::ostringstream str;
    str << "cuid: " << cuid << " Mitigations: " << mitigations.size() << " Acls: " << acls.size();
    return str.str();
}

bool ClientAgentStatus::sameTarget(MitigationStatistics *m1, MitigationStatistics *m2){
    //Check TargetPrefix
    if(m1->getTargetPrefixArraySize() != m2->getTargetPrefixArraySize())
        return false;
    for(int i = 0, n = m1->getTargetPrefixArraySize(); i < n; i++){
        if(strcmp(m1->getTargetPrefix(i), m2->getTargetPrefix(i)))
           return false;
    }
    //Check PortRange
    if(m1->getPortRangeArraySize() != m2->getPortRangeArraySize())
        return false;
    for(int i = 0, n = m1->getPortRangeArraySize(); i < n; i++){
        PortRange pr1 = m1->getPortRange(i),pr2 = m2->getPortRange(i);
        if(pr1.lowerPort != pr2.lowerPort || pr1.op != pr2.op || pr1.upperPort != pr2.upperPort)
            return false;
    }
    //Check Protocol
    if(m1->getTargetProtocolArraySize() != m2->getTargetProtocolArraySize())
        return false;
    for(int i = 0, n = m1->getTargetProtocolArraySize(); i < n; i++)
        if(m1->getTargetProtocol(i) != m2->getTargetProtocol(i))
            return false;
    //Check Fqdn
    if(m1->getTargetFqdnArraySize() != m2->getTargetFqdnArraySize())
        return false;
    for(int i = 0, n = m1->getTargetFqdnArraySize(); i < n; i++)
        if(strcmp(m1->getTargetFqdn(i), m2->getTargetFqdn(i)))
            return false;
    //Check Uri
    if(m1->getTargetUriArraySize() != m2->getTargetUriArraySize())
        return false;
    for(int i = 0, n = m1->getTargetUriArraySize(); i < n; i++)
        if(strcmp(m1->getTargetUri(i), m2->getTargetUri(i)))
            return false;
    //Check AliasName
    if(strcmp(m1->getAliasName(), m2->getAliasName()))
        return false;
    return true;
}

MitigationStatistics *ClientAgentStatus::createToStatistics(const inet::Ptr<const MitigationCreate> & c){
    MitigationStatistics *s = new MitigationStatistics();
    int i, n = c->getTargetPrefixArraySize();
    for(i = 0, s->setTargetPrefixArraySize(n); i < n; i++)
        s->setTargetPrefix(i, c->getTargetPrefix(i));
    n = c->getPortRangeArraySize();
    for(i = 0, s->setPortRangeArraySize(n); i < n; i++)
        s->setPortRange(i, c->getPortRange(i));
    n = c->getTargetProtocolArraySize();
    for(i = 0, s->setTargetProtocolArraySize(n); i < n; i++)
        s->setTargetProtocol(i, c->getTargetProtocol(i));
    n = c->getTargetFqdnArraySize();
    for(i = 0, s->setTargetFqdnArraySize(n); i < n; i++)
        s->setTargetFqdn(i, c->getTargetFqdn(i));
    n = c->getTargetUriArraySize();
    for(i = 0, s->setTargetUriArraySize(n); i < n; i++)
        s->setTargetUri(i, c->getTargetUri(i));
    s->setAliasName(c->getAliasName());
    s->setLifetime(c->getLifetime());
    s->setTriggerMitigation(c->getTriggerMitigation());
    if(c->getTriggerMitigation())
        s->setStatus(SETUP);
    else
        s->setStatus(STANDING_BY);
    s->setMitigationStart(simTime().raw());
    s->setBytesDropped(0);
    s->setBpsDropped(0);
    s->setPktsDropped(0);
    s->setPpsDropped(0);
    return s;
}

bool ClientAgentStatus::createMitigations(std::string mid, MitigationStatistics * m){
    for(auto elem : mitigations){
        if(sameTarget(elem.second, m)){
            return false;
        }
    }
    mitigations.insert(std::pair<std::string, MitigationStatistics *>(mid, m));
    return true;
}

MitigationStatistics *ClientAgentStatus::getMitigations(std::string mid){
    auto it = mitigations.find(mid);
    if(it != mitigations.end())
        return it->second;
    else{
        EV_INFO << "ClientAgentStatus(): No Mitigation Found!";
        return nullptr;
    }
}

MitigationStatistics *ClientAgentStatus::deleteMitigations(std::string mid){
    auto it = mitigations.find(mid);
    if(it != mitigations.end()){
        mitigations.erase(it);
        return it->second;
    }
    else{
        EV_INFO << "ClientAgentStatus(): No Mitigation Found!";
        return nullptr;
    }
}

void ClientAgentStatus::setMitigationStatus(std::string mid, MitigationStatus status){
    auto it = mitigations.find(mid);
    if(it != mitigations.end())
        it->second->setStatus(status);
    else
        EV_INFO << "ClientAgentStatus(): No Mitigation Found!";
}

bool ClientAgentStatus::createAcl(Acl* acl){
    acls.push_back(*acl);
    return true;
}

Acl *ClientAgentStatus::getAcl(std::string name){
    for(int i=0; i<acls.size(); i++)
        if(!strcmp(acls[i].getName(), name.c_str()))
            return &acls[i];
    return nullptr;
}

void ClientAgentStatus::deleteAcl(std::string name){
    for(auto i=acls.begin(); i<=acls.end(); i++){
        if(!strcmp(i->getName(), name.c_str())){
            acls.erase(i);
            return ;
        }
    }
}

