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

#ifndef APPLICATIONS_AGENTQUARTET_CLIENTAGENT_CLIENTAGENT_H_
#define APPLICATIONS_AGENTQUARTET_CLIENTAGENT_CLIENTAGENT_H_

#include <inet/common/INETDefs.h>
#include <inet/common/lifecycle/ILifecycle.h>
#include <inet/common/lifecycle/LifecycleOperation.h>
#include <inet/common/lifecycle/NodeStatus.h>
#include <inet/applications/base/ApplicationBase.h>
#include <inet/transportlayer/contract/tcp/TcpSocket.h>
#include <inet/applications/tcpapp/TcpAppBase.h>
#include "../common/AQCommon.h"
#include "../ClientAgent/MitigationInfo.h"

enum SelfMsgType{
    INIT = 0,
    RETRANSMIT_TIMEOUT = 1,
    HEARTBEAT_TIMEOUT = 2,
    STOP = 3
    //To Add
};

using namespace inet;

class ClientAgent : public TcpAppBase{
  private:
    simtime_t tStart;
    simtime_t tStop;
    simtime_t retransmitTimeout;
    simtime_t efficacyUpdateTimeOut;
    cMessage *timeoutMsg = nullptr;
    int cuid = getSimulation()->getUniqueNumber();//ToDo: Encryption for cuid
    bool peerHbStatus = false;
    int mid = 0;
    //informations of Mitigations
    typedef std::vector<MitigationInfo> Infos;
    Infos infos;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage * msg) override;
    virtual void handleTimer(cMessage *msg) override;

    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void close() override;

    virtual void handleResponse(Packet *packet);
    virtual void handleDetection(Packet *packet);

    //AQ Methods
    //AQ Client
    virtual void regist();
    //AQ FilterRule
    virtual void sendProtectList();
    //AQ Mitigation
    virtual void createMitigation(std::string prefix, int32_t lifetime, bool trigger);
    virtual void deleteMitigation(std::string mid);
    virtual void updateMitigationEfficacy(std::string mid, bool attackStatus);
    //AQ Heartbeat
    virtual void sendHeartbeat();
};

#endif /* APPLICATIONS_AGENTQUARTET_CLIENTAGENT_CLIENTAGENT_H_ */
