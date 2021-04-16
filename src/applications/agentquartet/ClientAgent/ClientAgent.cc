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

#include "ClientAgent.h"

#include <inet/common/ModuleAccess.h>
#include <inet/common/ProtocolTag_m.h>
#include <inet/common/lifecycle/NodeStatus.h>
#include <inet/common/packet/Message.h>
#include <inet/common/lifecycle/ModuleOperations.h>
#include <inet/common/packet/Packet.h>
#include <inet/common/TimeTag_m.h>
#include <inet/common/XMLUtils.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/applications/common/SocketTag_m.h>
#include "applications/agentquartet/DetectAgent/DetectorMsgs_m.h"

using namespace inet;

Define_Module(ClientAgent);

std::ostream& operator<<(std::ostream& os, const MitigationInfo& mi){
    os << mi.str();
    return os;
};

void ClientAgent::initialize(int stage){
    TcpAppBase::initialize(stage);
    if(stage == INITSTAGE_LOCAL){
        tStart = par("tStart");
        tStop = par("tStop");
        retransmitTimeout = par("retransmitTimeout");
        efficacyUpdateTimeOut = par("efficacyUpdateTimeOut");
        timeoutMsg = new cMessage("timer");
        WATCH(peerHbStatus);
        WATCH_VECTOR(infos);
    }
}

void ClientAgent::handleMessageWhenUp(cMessage *msg){
    if (msg->isSelfMessage())
        handleTimer(msg);
    else if(msg->arrivedOn("detectIn"))
        handleDetection(check_and_cast<Packet*>(msg));
    else
        socket.processMessage(msg);
}

void ClientAgent::handleTimer(cMessage *msg){
    //retransmit
    switch(msg->getKind()){
    case INIT:
        connect();
        break;
    case RETRANSMIT_TIMEOUT:{
        /*scheduleAt(simTime()+retransmitTimeout ,timeoutMsg);
        sendPacket(p);*/
        //retransmit
        break;
    }
    case HEARTBEAT_TIMEOUT:
        //heartbeat lost
        break;
    case STOP:
        close();
        break;
    default:
        throw cRuntimeError("Invaild timer msg  kind");
    }
    delete msg;
}

void ClientAgent::socketEstablished(TcpSocket *socket){
    TcpAppBase::socketEstablished(socket);
    regist();
}

void ClientAgent::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent){
    packetsRcvd++;
    bytesRcvd += msg->getByteLength();
    emit(packetReceivedSignal, msg);
    handleResponse(check_and_cast<Packet *>(msg));
    delete msg;
}

void ClientAgent::socketClosed(TcpSocket *socket){
    TcpAppBase::socketClosed(socket);

}

void ClientAgent::socketFailure(TcpSocket *socket, int code){
    TcpAppBase::socketFailure(socket, code);
    //reconnect
}

void ClientAgent::handleStartOperation(LifecycleOperation *operation){
    timeoutMsg->setKind(INIT);
    scheduleAt(tStart, timeoutMsg);
}

void ClientAgent::handleStopOperation(LifecycleOperation *operation){
    if (socket.getState() == TcpSocket::CONNECTED || socket.getState() == TcpSocket::CONNECTING || socket.getState() == TcpSocket::PEER_CLOSED)
        close();
}

void ClientAgent::handleCrashOperation(LifecycleOperation *operation){
    if (operation->getRootModule() != getContainingNode(this))
        socket.destroy();
}

void ClientAgent::close(){
    //close
}

void ClientAgent::regist(){
    const auto& header = makeShared<AQRequestHeader>();
    header->setHeader(POST);
    header->setUriPathArraySize(2); // AQ / Client
    header->setUriPath(0, "AQ");
    header->setUriPath(1, "Client");
    header->setChunkLength(B(std::string("POST /AQ/Client").length()));
    const auto& data = makeShared<ClientRegister>();
    data->setCuid(std::to_string(cuid).c_str());
    data->setChunkLength(B(std::to_string(cuid).length()));
    //encapsule and send packet
    Packet *p = new Packet("ClientAgent-Client-Register");
    p->insertAtBack(header);
    p->insertAtBack(data);
    //dup = p->dup();
    sendPacket(p);
    this->getParentModule()->bubble("Register");
    //timeout retry schedule
}

void ClientAgent::handleResponse(Packet *packet){
    const auto& header = packet->peekAtFront<AQResponseHeader>();
    if(!strcmp(header->getUriPath(1),"Client")){
        switch(header->getHeader()){
        case CREATED:{
            std::string s = std::string("ClientAgent" + std::to_string(cuid) + ": registered");
            this->getParentModule()->bubble(s.c_str());
            //send FilterRuleInstall Request
            sendProtectList();
            break;
        }
        case DELETED:
            EV_INFO << "ClientAgent" << cuid << ": unregistered";
            break;
        default:
            //ToDo: error and other cases
            break;
        }
    }
    else if(!strcmp(header->getUriPath(1),"Mitigation")){
        switch(header->getHeader()){
        case CREATED:{
            std::string s = std::string("Mitigation Created:") +std::string(header->getUriPath(3));
            this->getParentModule()->bubble(s.c_str());
            break;
        }
        case CONTENT:
            EV_INFO << "ClientAgent" << cuid << ": Mitigation Detail Received. " << header->getUriPath(3);
            break;
        case CHANGED:
            EV_INFO << "ClientAgent" << cuid << ": Mitigation Effect Updated. " << header->getUriPath(3);
            break;
        case DELETED:
            EV_INFO << "ClientAgent" << cuid << ": Mitigation Deleted. " << header->getUriPath(3);
            break;
        default:
            //ToDo: error and other cases
            break;
        }
    }
    else if(!strcmp(header->getUriPath(1),"FilterRules")){
        switch(header->getHeader()){
        case CREATED:
            this->getParentModule()->bubble("FilterRules Created");
            break;
        case CONTENT:
            break;
        case DELETED:
            break;
        default:
            //ToDo: error and other cases
            break;
        }
    }
    else if(!strcmp(header->getUriPath(1),"Heardbeat")){
        if(header->getHeader() == CHANGED){
            EV_INFO << "ClientAgent" << cuid << ": Heartbeat Received. ";
        }
        else{
            //ToDo: error and other cases
        }
    }
    else{
        //ToDo: error and other cases
        //throw cRuntimeError("ClientAgent(): Unsupported Response Type or Error!");
    }
}

void ClientAgent::handleDetection(Packet *packet){
    //Mitigation Request
    const auto& detection = packet->peekAtFront<DetectorMsg>();
    for(int i = 0, n = infos.size(); i < n ; i++){
        if(infos[i].inTargets(std::string(detection->getTargetPrefix()))){
            if(detection->getAttackDetected()){
                this->getParentModule()->bubble("Detection Already Reported");
                //UpdateMitigationEfficacy(infos[i].getMid(), true);
                delete packet;
                return;
            }
            else{
                updateMitigationEfficacy(infos[i].getMid(), false);
                delete packet;
                return;
            }
        }
    }
    this->getParentModule()->bubble("New DDoS Attack Detection");
    createMitigation(detection->getTargetPrefix(), 3600, true);
    delete packet;
}

void ClientAgent::sendProtectList(){
    std::ostringstream headerStr, dataStr;
    const auto& header = makeShared<AQRequestHeader>();
    header->setHeader(POST);
    header->setUriPathArraySize(3); // AQ / Client
    header->setUriPath(0, "AQ");
    header->setUriPath(1, "FilterRules");
    header->setUriPath(2, std::string("cuid="+std::to_string(cuid)).c_str());
    headerStr << "POST AQ/FilterRules/cuid=" << cuid;
    header->setChunkLength(B(headerStr.str().length()));

    const auto& data = makeShared<Acls>();
    data->setChunkLength(B(0));
    Acl *tmp = new Acl();
    tmp->setName("ProtectList");
    tmp->setType("");//acl-type
    tmp->setPendingLifetime(-1);
    tmp->setActivitionType(ACTIVITE_WHEN_MITIGATING);

    cXMLElement *config = par("protectList");
    cXMLElementList protectList = config->getChildrenByTagName("item");
    tmp->setAceArraySize(protectList.size() * 2);

    int i = 0;
    for(auto & item :protectList){
        L3Address src, dest;
        L3AddressResolver().tryResolve(item->getAttribute("src"), src);
        L3AddressResolver().tryResolve(item->getAttribute("dest"), dest);
        std::ostringstream in, out;
        in << "ACCEPT; " << src.toIpv4().str(0) << "/32 > " << dest.toIpv4().str(0) << "/32; * > *";
        out << "ACCEPT; " << dest.toIpv4().str(0) << "/32 > " << src.toIpv4().str(0) << "/32; * > *";
        Ace *a1 = new Ace();
        Ace *a2 = new Ace();
        a1->setFilterRule(in.str().c_str());
        a2->setFilterRule(out.str().c_str());
        tmp->setAce(i++, *a1);
        tmp->setAce(i++, *a2);
        data->addChunkLength(B(in.str().length() + out.str().length()));
    }
    dataStr << "ProtectList" << "" << "-1" << "1" ; //name Type lifetime ActivitionType aces
    data->addChunkLength(B(dataStr.str().length()));
    data->setAclArraySize(1);
    data->setAcl(0, *tmp);
    //encapsule and send packet
    Packet *p = new Packet("ClientAgent-FilterRules-Install");
    p->insertAtFront(header);
    p->insertAtBack(data);
    sendPacket(p);
}

void ClientAgent::createMitigation(std::string prefix, int32_t lifetime, bool trigger){
    std::ostringstream headerStr, dataStr;
    const auto& header = makeShared<AQRequestHeader>();
    header->setHeader(POST);
    header->setUriPathArraySize(4); // AQ / Client
    header->setUriPath(0, "AQ");
    header->setUriPath(1, "Mitigation");
    header->setUriPath(2, std::string("cuid="+std::to_string(cuid)).c_str());
    header->setUriPath(3, std::string("mid="+std::to_string(++mid)).c_str());
    headerStr << "POST /AQ/Mitigation/cuid=" << cuid << "/mid=" << mid;
    header->setChunkLength(B(headerStr.str().length()));
    const auto& data = makeShared<MitigationCreate>();
    //check prefix
    data->setTargetPrefixArraySize(1);
    data->setTargetPrefix(0, prefix.c_str());
    data->setLifetime(lifetime);
    data->setTriggerMitigation(trigger);
    dataStr << prefix << lifetime << trigger;
    data->setChunkLength(B(dataStr.str().length()));
    Packet *p = new Packet("ClientAgent-Mitigation-Create");
    p->insertAtFront(header);
    p->insertAtBack(data);
    sendPacket(p);
    //change MitigationInfo at Client
    MitigationInfo *m = new MitigationInfo();
    m->setCreateTime(simTime());
    m->setMid(std::to_string(mid));
    m->addTarget(prefix);
    infos.push_back(*m);
}

void ClientAgent::deleteMitigation(std::string mid){
    std::ostringstream headerStr;
    const auto& header = makeShared<AQRequestHeader>();
    header->setHeader(DEL);
    header->setUriPathArraySize(4); // AQ / Client
    header->setUriPath(0, "AQ");
    header->setUriPath(1, "Mitigation");
    header->setUriPath(2, std::string("cuid="+std::to_string(cuid)).c_str());
    header->setUriPath(3, std::string("mid="+mid).c_str());
    headerStr << "DELETE /AQ/Mitigation/cuid=" << cuid << "/mid=" << mid ;
    header->setChunkLength(B(headerStr.str().length()));
    Packet *p = new Packet("ClientAgent-Mitigation-Delete");
    p->insertAtFront(header);
    sendPacket(p);
    //change state at Client
}

void ClientAgent::updateMitigationEfficacy(std::string mid, bool attackStatus){
    std::ostringstream headerStr, dataStr;
    const auto& header = makeShared<AQRequestHeader>();
    header->setHeader(PUT);
    header->setUriPathArraySize(4);
    header->setUriPath(0, "AQ");
    header->setUriPath(1, "Mitigation");
    header->setUriPath(2, std::string("cuid="+std::to_string(cuid)).c_str());
    header->setUriPath(3, std::string("mid="+mid).c_str());
    headerStr << "PUT /AQ/Mitigation/cuid=" << cuid << "/mid=" << mid ;
    header->setChunkLength(B(headerStr.str().length()));
    const auto& data = makeShared<MitigationEfficacy>();
    //check prefix
    data->setAttackStatus(attackStatus);
    Packet *p = new Packet("ClientAgent-Mitigation-UpdateEfficacy");
    dataStr << (attackStatus ? "true" : "false");
    data->setChunkLength(B(dataStr.str().length()));
    p->insertAtFront(header);
    p->insertAtBack(data);
    sendPacket(p);
}

void ClientAgent::sendHeartbeat(){
    std::ostringstream headerStr, dataStr;
    const auto& header = makeShared<AQRequestHeader>();
    header->setHeader(PUT);
    header->setUriPathArraySize(2);
    header->setUriPath(0, "AQ");
    header->setUriPath(1, "Heartbeat");
    headerStr << "PUT /AQ/Heartbeat";
    header->setChunkLength(B(headerStr.str().length()));
    const auto& data = makeShared<Heartbeat>();
    data->setPeerHbStatus(peerHbStatus);
    dataStr << (peerHbStatus  ? "true" : "false");
    data->setChunkLength(B(dataStr.str().length()));
    Packet *p = new Packet("ClientAgent-Heartbeat");
    p->insertAtFront(header);
    p->insertAtBack(data);
    sendPacket(p);
}

