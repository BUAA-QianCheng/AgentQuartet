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

#include "ServerAgent.h"

#include "applications/agentquartet/RouterAgent/RouterAgentMsgs_m.h"
#include "../common/AQCommon.h"

#include <inet/common/XMLUtils.h>
#include <inet/applications/base/ApplicationBase.h>

using namespace inet;

Define_Module(ServerAgent);
Define_Module(ServerAgentThread);

std::ostream& operator<<(std::ostream& os, const AreaInfo& ai){
    os << ai.str();
    return os;
};

std::ostream& operator<<(std::ostream& os, const ClientAgentStatus& s){
    os << s.str();
    return os;
};

void ServerAgent::initialize(int stage) {
    TcpServerHostApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        bytesRcvd = 0;
        WATCH(bytesRcvd);
        WATCH_PTRVECTOR(areaInfoList);
        WATCH_PTRVECTOR(status);
        //Watch status areaInfo connInfo ...
    }
    else if(stage == INITSTAGE_APPLICATION_LAYER){
        readAreaInfo();
    }
}

void ServerAgent::refreshDisplay() const {
    ApplicationBase::refreshDisplay();

    char buf[160];
    sprintf(buf, "connections: %d\nrcvd: %ld bytes", socketMap.size(), bytesRcvd);
    getDisplayString().setTagArg("t", 0, buf);
}


void ServerAgent::finish(){
    TcpServerHostApp::finish();
    recordScalar("bytesRcvd", bytesRcvd);
}

void ServerAgent::readAreaInfo(){
    using namespace xmlutils;

    cXMLElement *config = par("AreaInfoList");
    cXMLElementList areaInfos = config->getChildrenByTagName("areainfo");
    for(auto & areaInfo : areaInfos){
        AreaInfo *tmp = new AreaInfo();
        L3Address addr, rtr;
        L3AddressResolver().tryResolve(areaInfo->getAttribute("address"), addr);
        L3AddressResolver().tryResolve(areaInfo->getAttribute("router"), rtr);
        tmp->setAddress(addr);
        tmp->setRouter(rtr);
        tmp->setPrefixLength(atoi(areaInfo->getAttribute("prefixLength")));
        this->areaInfoList.push_back(tmp);
    }
}

//ServerAgentThread

void ServerAgentThread::initialize(int stage){
    TcpServerThreadBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        bytesRcvd = 0;
        WATCH(bytesRcvd);
    }
}

void ServerAgentThread::established(){
    bytesRcvd = 0;
    switch(this->sock->getRemotePort()){
    case CLIENT_AGENT:
        serverAgentModule->getParentModule()->bubble("Client Agent Connected");
        break;
    case ROUTER_AGENT:
        serverAgentModule->getParentModule()->bubble("Router Agent Connected");
        break;
    default:
        //throw cRuntimeError("ServerAgentThread: Wrong remote Port number");
        break;
    }
}

void ServerAgentThread::dataArrived(Packet *pk, bool urgent)
{
    long packetLength = pk->getByteLength();
    bytesRcvd += packetLength;
    serverAgentModule->bytesRcvd += packetLength;
    if(this->getSocket()->getRemotePort() == ROUTER_AGENT){
        handleRouterAgentMsg(pk);
    }
    else if(this->getSocket()->getRemotePort() == CLIENT_AGENT){
        handleClientAgentMsg(pk);
    }
    else {
        //UNKNOWN
    }
    delete pk;
}

void ServerAgentThread::refreshDisplay() const
{
    std::ostringstream os;
    os << (sock ? TcpSocket::stateName(sock->getState()) : "NULL_SOCKET") << "\nrcvd: " << bytesRcvd << " bytes" ;
    switch(this->sock->getRemotePort()){
        case CLIENT_AGENT:
            os << "\nAgentType: " <<"Client Agent";
            break;
        case ROUTER_AGENT:
            os << "\nAgentType: " <<"Router Agent";
            break;
        default:
            //throw cRuntimeError("ServerAgentThread: Wrong remote Port number");
        break;
        }
    os << "\nRemoteAddr: " << this->sock->getRemoteAddress().toIpv4().str();
    getDisplayString().setTagArg("t", 0, os.str().c_str());
}

//RouterAgent Methods
void ServerAgentThread::handleRouterAgentMsg(Packet *packet){
    //Receive and send to all routerAgents(related RouterAgents)
    const auto& data = packet->peekAtFront<RouterAgentMsg>();
    Packet *p = new Packet("ServerAgent-RouterAgentMsg");
    p->insertAtFront(data);
    sendPacketToAllRouterAgent(p);
}

void ServerAgentThread::sendPacketToAllRouterAgent(Packet *packet){
    //for(std::set<TcpServerThreadBase *>::iterator it = serverAgentModule->threadSet.begin(); it != serverAgentModule->threadSet.end(); it++){}
    for(TcpServerThreadBase * thread : serverAgentModule->threadSet){
        if(thread->getSocket()->getRemotePort() == ROUTER_AGENT){
            Packet *p = packet->dup();
            thread->getSocket()->send(p);
        }
    }
    delete packet;
}

void ServerAgentThread::sendPacketToRelatedRouterAgent(Packet *packet, std::vector<L3Address> addrList){
    //srcAddr.getNumMatchingPrefixBits(srcNetwork.toIpv4()) < srcNetworkPrefixLength
    for(auto addr : addrList){
        for(TcpServerThreadBase * thread : serverAgentModule->threadSet){
            if(thread->getSocket()->getRemoteAddress().matches(addr, 32)){
                Packet *p = packet->dup();
                thread->getSocket()->send(p);
                break;
            }
        }
    }
    delete packet;
}

//ClientAgent Methods
void ServerAgentThread::handleClientAgentMsg(Packet *packet){
    const auto& header = packet->popAtFront<AQRequestHeader>();
    //check Header
    if(header->getHeader() > DEL || header->getHeader() < GET){
        //Request Method error
        return;
    }
    if(strcmp(header->getUriPath(0),"AQ")){
        //Wrong with UriPath
        return;
    }
    if(header->getUriPathArraySize() < 2){
        //Wrong with UriPath
        return;
    }
    //classify
    if(!strcmp(header->getUriPath(1), "Client")){
        registResponse(packet, header->getHeader());
        return ;
    }
    if(!strcmp(header->getUriPath(1), "FilterRules")){
        filterRulesResponse(packet, header->getHeader());
        return ;
    }
    if(!strcmp(header->getUriPath(1), "Mitigation")){
        mitigationResponse(packet, header->getHeader(), std::string(header->getUriPath(3)));
        return ;
    }
    if(!strcmp(header->getUriPath(1), "Heartbeat")){
        //deal with heartbeat
        return ;
    }
}

void ServerAgentThread::registResponse(Packet *packet, RequestType method){
    const auto& header = makeShared<AQResponseHeader>();
    header->setUriPathArraySize(3);
    header->setUriPath(0, "AQ");
    header->setUriPath(1, "Client");
    switch(method){
    case POST:{
        //check s in ServerAgent
        const auto& clientRegister = packet->peekAtFront<ClientRegister>();
        this->cuid = std::string(clientRegister->getCuid());

        //create client agent status
        statusPtr = new ClientAgentStatus();
        statusPtr->setCuid(cuid);
        serverAgentModule->status.push_back(statusPtr);

        //response packet
        header->setHeader(CREATED);
        header->setUriPath(2, (std::string("cuid=")+cuid).c_str());
        header->setChunkLength(B((std::string("CREATED /AQ/Client/cuid=") + cuid).length()));
        Packet *p = new Packet("ServerAgent-Client-Register");
        p->insertAtFront(header);
        this->getSocket()->send(p);
        serverAgentModule->getParentModule()->bubble("Registered");

        //Create Rule for ClientAgent and ServerAgent
        const auto& data = makeShared<RouterAgentMsg>();
        data->setChunkLength(B(0));
        data->setType(CREATE_FILTERRULE);
        data->addChunkLength(B(std::string("CREATE_FILTERRULE").length()));
        data->setStrsArraySize(2);
        std::ostringstream in, out;
        in << "ACCEPT; " << this->sock->getLocalAddress().toIpv4().str(0) <<"/32 >" << this->sock->getRemoteAddress().toIpv4().str(0) << "/32; " << this->sock->getLocalPort() << " > " << this->sock->getRemotePort();
        out << "ACCEPT; " << this->sock->getRemoteAddress().toIpv4().str(0) <<"/32 >" << this->sock->getLocalAddress().toIpv4().str(0) << "/32; " << this->sock->getRemotePort() << " > " << this->sock->getLocalPort();
        data->setStrs(0, in.str().c_str());
        data->setStrs(1, out.str().c_str());
        Packet *pk = new Packet("ServerAgent-FilterRules-Deliver");
        pk->insertAtFront(data);
        this->sendPacketToAllRouterAgent(pk);
        break;
    }
    case DEL:{
        //check client in ServerAgent

        //header->setUriPath(2, "cuid=" + cuid);
        header->setChunkLength(B((std::string("DELETED /AQ/Client/cuid=") + cuid).length()));
        Packet *p = new Packet("ServerAgent-Client-Unregister");
        this->getSocket()->send(p);
        serverAgentModule->getParentModule()->bubble("Unregistered");
        break;
    }
    default:{
        //invalid method
        break;
    }
    }
}

void ServerAgentThread::filterRulesResponse(Packet *packet, RequestType method){
    const auto& header = makeShared<AQResponseHeader>();
    switch(method){
    case PUT:
    case POST:{
        header->setHeader(CREATED);
        header->setUriPathArraySize(5);
        header->setUriPath(0, "AQ");
        header->setUriPath(1, "FilterRules");
        header->setUriPath(2, "cuid="); // + cuid
        header->setUriPath(3, "acls");
        header->setUriPath(4, "acl="); // + name
        header->setChunkLength(B(std::string("CREATED /AQ/FilterRules/cuid=/acls/acl=").length()));
        Packet *p = new Packet("ServerAgent-FilterRules-Install");
        p->insertAtBack(header);
        this->getSocket()->send(p);
        serverAgentModule->getParentModule()->bubble("FilterRules Installed");
        const auto& data = makeShared<RouterAgentMsg>();
        data->setChunkLength(B(0));
        data->setType(CREATE_FILTERRULE);
        data->addChunkLength(B(std::string("CREATE_FILTERRULE").length()));
        //Deliver rules to router
        const auto& acls = packet->peekAtFront<Acls>();
        for(int i = 0, n = acls->getAclArraySize(), cnt = 0; i < n; i++){
            cnt += acls->getAcl(i).getAceArraySize();
            data->setStrsArraySize(cnt);
            for(int j = 0, m = acls->getAcl(i).getAceArraySize(); j < m; j++){
                std::string s = acls->getAcl(i).getAce(j).getFilterRule();
                data->setStrs((cnt-m+j), s.c_str());
                data->addChunkLength(B(s.length()));
            }
        }
        Packet *pk = new Packet("ServerAgent-FilterRules-Deliver");
        pk->insertAtFront(data);
        this->sendPacketToAllRouterAgent(pk);
        break;
    }
    case GET:
        break;
    case DEL:
        break;
    default:
        break;
    }
}

void ServerAgentThread::mitigationResponse(Packet *packet, RequestType method, std::string mid){
    const auto& header = makeShared<AQResponseHeader>();
    header->setUriPathArraySize(4);
    header->setUriPath(0, "AQ");
    header->setUriPath(1, "Mitigation");
    header->setUriPath(2, (std::string("cuid=") + cuid).c_str()); // + cuid
    header->setUriPath(3, mid.c_str());  // + mid
    std::string m = std::string(mid, 4, mid.length()-4);
    switch(method){
    case POST:{//Mitigation Create
        const auto& mitigation = packet->peekAtFront<MitigationCreate>();
        //read mitigations from mitigationCreate
        //create mitigation in server agent
        ClientAgentStatus *s = getStatus(cuid);
        if(s == nullptr){
            //ToDo: handle wrong cuid
        }
        if(s->createMitigations(m, ClientAgentStatus().createToStatistics(mitigation))){
            serverAgentModule->getParentModule()->bubble("Mitigation Created");
            header->setHeader(CREATED);
            header->setChunkLength(B(std::string("CREATED /AQ/Mitigation/cuid="  + cuid  + mid).length()));
            Packet *p = new Packet("ServerAgent-Mitigation-Created");
            p->insertAtBack(header);
            this->getSocket()->send(p);

            //change FilterType in target area
            const auto& msg = makeShared<RouterAgentMsg>();
            msg->setType(TO_ACCEPTLIST_ONLY_FILTERTYPE);
            msg->setChunkLength(B(std::string("TO_ACCEPTLIST_ONLY_FILTERTYPE").length()));
            Packet *pk = new Packet("ServerAgent-FilterType-Change");
            pk->insertAtFront(msg);
            this->sendPacketToRelatedRouterAgent(pk, this->searchTargetInAreaInfoList(m));
            break;
        }
        else{
            serverAgentModule->getParentModule()->bubble("Mitigation Target Conflict");
            header->setHeader(BAD_REQUEST);
            header->setChunkLength(B((std::string("CREATED /AQ/Mitigation/cuid=") + cuid  + mid).length()));
            Packet *p = new Packet("ServerAgent-Mitigation-Conflict");
            p->insertAtBack(header);
            this->getSocket()->send(p);
            break;
        }
    }
    case PUT:{//Mitigation Efficacy Update
            const auto& efficacy = packet->peekAtFront<MitigationEfficacy>();
            if(efficacy->getAttackStatus()){
                //to do if still under attack
                std::ostringstream str;
                str << "Efficacy Updated: " << mid << " under attack" ;
                serverAgentModule->getParentModule()->bubble(str.str().c_str());
            }
            else{
                //to do if attack stopped
                std::ostringstream str;
                str << "Efficacy Updated: " << mid << " attack mitigated" ;
                serverAgentModule->getParentModule()->bubble(str.str().c_str());
                statusPtr->setMitigationStatus(m, MITIGATED);
                //change FilterType in target area
                const auto& msg = makeShared<RouterAgentMsg>();
                msg->setType(TO_DROPLIST_ONLY_FILTERTYPE);
                msg->setChunkLength(B(std::string("TO_DROPLIST_ONLY_FILTERTYPE").length()));
                Packet *pk = new Packet("ServerAgent-FilterType-Change");
                pk->insertAtFront(msg);
                this->sendPacketToRelatedRouterAgent(pk, this->searchTargetInAreaInfoList(m));
            }
            header->setHeader(CHANGED);
            //get mitigation statistic from ServerAgent with mid
            header->setChunkLength(B(std::string("CHANGED /AQ/Mitigation/cuid=" + cuid + "/" + mid).length()));
            Packet *p = new Packet("ServerAgent-Mitigation-Efficacy-Updated");
            p->insertAtFront(header);
            this->getSocket()->send(p);
        break;
    }

    case GET:{//Mitigation Query
        break;
    }
    case DEL:{//Mitigation Delete
        //check mitigation with mid
        //delete mitigation
        statusPtr->deleteMitigations(m);
        header->setHeader(DELETED);
        header->setUriPath(3, mid.c_str());
        header->setChunkLength(B(std::string("DELETED /AQ/Mitigation/cuid=" + cuid + "/" + mid).length()));
        Packet *p = new Packet("ServerAgent-Mitigation-Deleted");
        p->insertAtBack(header);
        this->getSocket()->send(p);
        break;
    }
    default:
        break;
    }
}

ClientAgentStatus* ServerAgentThread::getStatus(std::string cuid){
    for(auto& s : serverAgentModule->status){
        if(s->getCuid() == cuid)
            return s;
    }
    return nullptr;
}

std::vector<L3Address> ServerAgentThread::searchTargetInAreaInfoList(std::string mid){
    //return a routerAddressList that msgs to be sent
    MitigationStatistics *m = statusPtr->getMitigations(mid);
    std::vector<L3Address> routerAddressList;
    for(int i = 0, n = m->getTargetPrefixArraySize(); i < n; i++){
        for(int j = 0, k = serverAgentModule->areaInfoList.size(); j < k; j++){
            AreaInfo *ai = serverAgentModule->areaInfoList[j];
            if(ai->getAddress().matches(L3Address(Ipv4Address(m->getTargetPrefix(i))), ai->getPrefixLength()))
                routerAddressList.push_back(ai->getRouter());
        }
    }
    return routerAddressList;
}
