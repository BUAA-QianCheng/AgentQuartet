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

#include "RouterAgent.h"

#include "applications/agentquartet/common/AQCommon.h"
#include "applications/agentquartet/RouterAgent/RouterAgentMsgs_m.h"
#include "networklayer/FilterRuleTable.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/applications/common/SocketTag_m.h"

using namespace inet;

Define_Module(RouterAgent);

void RouterAgent::initialize(int stage){
    TcpAppBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        numRequestsToSend = 0;
        earlySend = false;    // TBD make it parameter
        WATCH(numRequestsToSend);
        WATCH(earlySend);

        frt = getModuleFromPar<FilterRuleTable>(par("filterRuleTableModule"),this);
        rt = getModuleFromPar<Ipv4RoutingTable>(par("routingTableModule"), this);

        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");
    }
}

void RouterAgent::handleStartOperation(LifecycleOperation *operation){
    simtime_t now = simTime();
    simtime_t start = std::max(startTime, now);
    scheduleAt(start, timeoutMsg);
}

void RouterAgent::handleStopOperation(LifecycleOperation *operation){
    cancelAndDelete(timeoutMsg);
    if (socket.getState() == TcpSocket::CONNECTED || socket.getState() == TcpSocket::CONNECTING || socket.getState() == TcpSocket::PEER_CLOSED)
        close();
}

void RouterAgent::handleCrashOperation(LifecycleOperation *operation){
    cancelAndDelete(timeoutMsg);
    if (operation->getRootModule() != getContainingNode(this))
        socket.destroy();
}


void RouterAgent::handleTimer(cMessage *msg){
    connect();
}

void RouterAgent::socketEstablished(TcpSocket *socket){
    TcpAppBase::socketEstablished(socket);
    std::ostringstream str;
    str << "Server Agent Connected! Connect Address: " << this->socket.getLocalAddress().toIpv4().str();
    this->getParentModule()->bubble(str.str().c_str());
    EV_INFO << str.str();

    //Add accept rules of RouterAgent and ServerAgent
    const char *localAddress = par("localAddress");
    int localPort = ROUTER_AGENT;
    const char *connectAddress = par("connectAddress");
    int connectPort = SERVER_AGENT;
    L3Address routerAddr, serverAddr;
    L3AddressResolver().tryResolve(localAddress, routerAddr);
    L3AddressResolver().tryResolve(connectAddress, serverAddr);
    std::ostringstream in, out;
    in << "ACCEPT; "<< serverAddr.toIpv4().str(0) << "/32 > " << routerAddr.toIpv4().str(0) << "/32; " << connectPort << " > " << localPort;
    out<< "ACCEPT; " << routerAddr.toIpv4().str(0) << "/32 > " << serverAddr.toIpv4().str(0) << "/32; " << localPort << " > " << connectPort;
    FilterRule *i = new FilterRule(in.str());
    FilterRule *o = new FilterRule(out.str());
    frt->addFilterRule(i);
    frt->addFilterRule(o);
}

void RouterAgent::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent){
    packetsRcvd++;
    bytesRcvd += msg->getByteLength();
    emit(packetReceivedSignal, msg);
    //read packet and act
    handleMsgFromServerAgent(msg);
    delete msg;
}

void RouterAgent::close(){
    TcpAppBase::close();
}

void RouterAgent::socketClosed(TcpSocket *socket){
    TcpAppBase::socketClosed(socket);
}

void RouterAgent::socketFailure(TcpSocket *socket, int code){
    TcpAppBase::socketFailure(socket, code);
}

void RouterAgent::handleMsgFromServerAgent(Packet *packet){
    const auto& data = packet->peekAtFront<RouterAgentMsg>();
    switch(data->getType()){
    case CREATE_FILTERRULE:{
        for(int i=0, n=data->getStrsArraySize(); i < n ; i++){
            FilterRule *entry = new FilterRule(data->getStrs(i));
            frt->addFilterRule(entry);
        }
        this->getParentModule()->bubble("FilterRule Created");
        break;
    }
    case DELETE_FILTERRULE:{
        this->getParentModule()->bubble("FilterRule Deleted");
        break;
    }
    case CREATE_ROUTE:{
        this->getParentModule()->bubble("Route Created");
        break;
    }
    case CHANGE_ROUTE:{
        this->getParentModule()->bubble("Route Changed");
        break;
    }
    case DELETE_ROUTE:{
        this->getParentModule()->bubble("Route Deleted");
        break;
    }
    case TO_IDLE_FILTER_TYPE:{
        frt->setFilterType(IDLE);
        this->getParentModule()->bubble("FilterType: IDLE");
        break;
    }
    case TO_DROPLIST_ONLY_FILTERTYPE:{
        this->getParentModule()->bubble("FilterType: DROPLIST_ONLY");
        frt->setFilterType(DROPLIST_ONLY);
        break;
    }
    case TO_ACCEPTLIST_ONLY_FILTERTYPE:{
        this->getParentModule()->bubble("FilterType: ACCEPTLIST_ONLY");
        frt->setFilterType(ACCEPTLIST_ONLY);
        break;
    }
    default:
        throw cRuntimeError("handleMsgFromServerAgent(): Wrong RouterAgentMsg Type!");
    }
}

void RouterAgent::sendFilterRuleToServerAgent(std::string rule){
    Enter_Method_Silent();
    const auto& data = makeShared<RouterAgentMsg>();
    data->setType(CREATE_FILTERRULE);
    data->setStrsArraySize(1);
    data->setStrs(0, rule.c_str());
    data->setChunkLength(B(rule.length()));
    Packet *p = new Packet("RouterAgent-FilterRule-Create");
    p->insertAtBack(data);
    sendPacket(p);
}

