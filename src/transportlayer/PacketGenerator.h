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

#ifndef TRANSPORTLAYER_PACKETGENERATOR_H_
#define TRANSPORTLAYER_PACKETGENERATOR_H_


#include <omnetpp.h>

#include <inet/common/INETDefs.h>
#include <inet/common/lifecycle/ModuleOperations.h>
#include <inet/common/packet/Packet.h>
#include <inet/networklayer/common/L3Address.h>
#include <inet/transportlayer/base/TransportProtocolBase.h>
#include <inet/transportlayer/common/CrcMode_m.h>
#include <inet/transportlayer/tcp/TcpConnection.h>
#include <inet/transportlayer/tcp_common/TcpCrcInsertionHook.h>

using namespace inet;
using namespace tcp;

class UdpCrcInsertion : public NetfilterBase::HookBase {
  public:
    virtual Result datagramPreRoutingHook(Packet *packet) override { return ACCEPT; }
    virtual Result datagramForwardHook(Packet *packet) override { return ACCEPT; }
    virtual Result datagramPostRoutingHook(Packet *packet) override;
    virtual Result datagramLocalInHook(Packet *packet) override { return ACCEPT; }
    virtual Result datagramLocalOutHook(Packet *packet) override { return ACCEPT; }
};

class PacketGenerator : public TransportProtocolBase {
  protected:
    CrcMode crcMode = CRC_MODE_UNDEFINED;
    TcpCrcInsertion tcpCrcInsertion;
    UdpCrcInsertion udpCrcInsertion;

    L3Address localAddr;
    L3Address destAddr;
    int localPort;
    int destPort;
    const char *packetName = nullptr;
    Packet *packetToSend = nullptr;
    int packetLength;
    simtime_t startTime;
    simtime_t stopTime;
    cMessage *selfMsg = nullptr;

    long numSent = 0;

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; };
    virtual void finish() override;
    virtual void refreshDisplay() const override{ OperationalBase::refreshDisplay();};

    virtual void handleMessageWhenUp(cMessage* msg) override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override{ cancelEvent(selfMsg); };
    virtual void handleCrashOperation(LifecycleOperation *operation) override{ cancelEvent(selfMsg); };

    virtual void sendPacket(){ Packet *p = packetToSend->dup(); send(p, "ipOut"); numSent++;}
  public:
    ~PacketGenerator(){ delete packetToSend; }
};

#endif /* TRANSPORTLAYER_PACKETGENERATOR_H_ */
