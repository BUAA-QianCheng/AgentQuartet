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

#ifndef APPLICATIONS_AGENTQUARTET_ROUTERAGENT_ROUTERAGENT_H_
#define APPLICATIONS_AGENTQUARTET_ROUTERAGENT_ROUTERAGENT_H_

#include "networklayer/FilterRuleTable.h"

#include <inet/common/INETDefs.h>
#include <inet/common/socket/SocketMap.h>
#include <inet/common/lifecycle/ILifecycle.h>
#include <inet/common/lifecycle/LifecycleOperation.h>
#include <inet/common/lifecycle/NodeStatus.h>
#include <inet/networklayer/ipv4/Ipv4RoutingTable.h>
#include <inet/transportlayer/contract/tcp/TcpSocket.h>
#include <inet/applications/base/ApplicationBase.h>
#include <inet/applications/tcpapp/TcpAppBase.h>
#include <inet/applications/tcpapp/TcpServerHostApp.h>

using namespace inet;

class RouterAgent : public TcpAppBase{
  protected:
    cMessage *timeoutMsg = nullptr;
    bool earlySend = false;    // if true, don't wait with sendRequest() until established()
    int numRequestsToSend = 0;    // requests to send in this session
    simtime_t startTime;
    simtime_t stopTime;

    FilterRuleTable* frt;
    Ipv4RoutingTable* rt;

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleTimer(cMessage *msg) override;

    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void close() override;

    virtual void handleMsgFromServerAgent(Packet * packet);
  public:
    ~RouterAgent(){ delete timeoutMsg; }
    virtual void sendFilterRuleToServerAgent(std::string rule);

};

#endif /* APPLICATIONS_AGENTQUARTET_ROUTERAGENT_ROUTERAGENT_H_ */
