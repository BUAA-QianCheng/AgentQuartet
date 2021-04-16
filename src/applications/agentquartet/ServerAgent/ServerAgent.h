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

#ifndef APPLICATIONS_AGENTQUARTET_SERVERAGENT_SERVERAGENT_H_
#define APPLICATIONS_AGENTQUARTET_SERVERAGENT_SERVERAGENT_H_

#include <inet/common/INETDefs.h>
#include <inet/common/socket/SocketMap.h>
#include <inet/common/lifecycle/ILifecycle.h>
#include <inet/common/lifecycle/LifecycleOperation.h>
#include <inet/common/lifecycle/NodeStatus.h>
#include <inet/applications/tcpapp/TcpServerHostApp.h>
#include "applications/agentquartet/ServerAgent/AreaInfo.h"
#include "applications/agentquartet/ServerAgent/ClientAgentStatus.h"

class ServerAgent : public TcpServerHostApp{
  protected:
    long bytesRcvd = 0;
    typedef std::vector<ClientAgentStatus *> ClientAgentStatusList;
    ClientAgentStatusList status;
    typedef std::vector<AreaInfo *> AreaInfoList;
    AreaInfoList areaInfoList;
  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void finish() override;
    virtual void refreshDisplay() const override;

  protected:
    //AreaInfo
    virtual void readAreaInfo();

    friend class ServerAgentThread;
};

class ServerAgentThread : public TcpServerThreadBase {
  protected:
    long bytesRcvd = 0;
    ServerAgent *serverAgentModule = nullptr;
    ClientAgentStatus *statusPtr = nullptr;
    std::string cuid;

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void refreshDisplay() const override;

    virtual void established() override;
    virtual void dataArrived(Packet *msg, bool urgent) override;
    virtual void timerExpired(cMessage *timer) override { throw cRuntimeError("Model error: unknown timer message arrived"); }
    virtual void init(TcpServerHostApp *hostmodule, TcpSocket *socket) override { TcpServerThreadBase::init(hostmodule, socket); serverAgentModule = check_and_cast<ServerAgent *>(hostmod); }

    //RouterAgent Methods
    virtual void handleRouterAgentMsg(Packet *packet);
    virtual void sendPacketToAllRouterAgent(Packet *packet);
    virtual void sendPacketToRelatedRouterAgent(Packet * packet,  std::vector<L3Address> addrList);

    //ClientAgent Methods
    virtual void handleClientAgentMsg(Packet *packet);
    virtual void registResponse(Packet *packet, RequestType method);
    virtual void filterRulesResponse(Packet *packet, RequestType method);
    virtual void mitigationResponse(Packet *packet, RequestType method, std::string mid);

    //ClientAgentStatus
    virtual ClientAgentStatus *getStatus(std::string cuid);
    virtual std::vector<L3Address> searchTargetInAreaInfoList(std::string m);
};

#endif /* APPLICATIONS_AGENTQUARTET_SERVERAGENT_SERVERAGENT_H_ */
