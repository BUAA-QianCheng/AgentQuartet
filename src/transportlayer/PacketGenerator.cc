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

#include "PacketGenerator.h"

#include <iostream>

#include <inet/common/checksum/TcpIpChecksum.h>
#include <inet/common/TimeTag_m.h>
#include <inet/common/lifecycle/LifecycleOperation.h>
#include <inet/common/lifecycle/ModuleOperations.h>
#include <inet/common/ProtocolTag_m.h>

#include <inet/linklayer/common/InterfaceTag_m.h>

#include <inet/networklayer/ipv4/Icmp.h>
#include <inet/networklayer/contract/IL3AddressType.h>
#include <inet/networklayer/common/L3Address.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/common/L3AddressTag_m.h>
#include <inet/networklayer/common/L3Tools.h>
#include <inet/networklayer/common/IpProtocolId_m.h>

#include <inet/transportlayer/tcp_common/TcpHeader.h>
#include <inet/transportlayer/tcp/Tcp.h>
#include <inet/transportlayer/tcp/TcpConnection.h>
#include <inet/transportlayer/tcp_common/TcpCrcInsertionHook.h>
#include <inet/transportlayer/common/L4Tools.h>
#include <inet/transportlayer/common/CrcMode_m.h>
#include <inet/transportlayer/common/L4PortTag_m.h>
#include <inet/transportlayer/common/TransportPseudoHeader_m.h>
#include <inet/transportlayer/contract/udp/UdpControlInfo.h>
#include <inet/transportlayer/udp/Udp.h>
#include <inet/transportlayer/udp/UdpHeader_m.h>

#include <inet/applications/common/SocketTag_m.h>
#include <inet/applications/base/ApplicationPacket_m.h>

using namespace inet;
using namespace tcp;

Define_Module(PacketGenerator);

void PacketGenerator::initialize(int stage){
    OperationalBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        const char *crcModeString = par("crcMode");
        crcMode = parseCrcMode(crcModeString, true);

        localPort = par("localPort");
        destPort = par("destPort");
        startTime = par("startTime");
        stopTime = par("stopTime");
        packetName = par("packetName");
        selfMsg = new cMessage("sendTimer");
        packetLength = par("packetLength");
    }
    else if(stage == INITSTAGE_TRANSPORT_LAYER){
        localAddr = L3AddressResolver().resolve(par("localAddr"));
        destAddr = L3AddressResolver().resolve(par("destAddr"));

        //generate packet here
        const char*pktType = par("packetType");
        packetToSend = new Packet(packetName);

        if(strstr(pktType, "TCP") != nullptr){ //TCP PACKET

            ASSERT(destAddr.getType() == L3Address::IPv4);
            const auto& tcpseg = makeShared<TcpHeader>();
            IL3AddressType *addressType = destAddr.getAddressType();
            packetToSend->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(addressType->getNetworkProtocol());
            auto addresses = packetToSend->addTagIfAbsent<L3AddressReq>();
            addresses->setSrcAddress(localAddr);
            addresses->setDestAddress(destAddr);
            //set flags
            if(strstr(pktType, "NULL") != nullptr){
                //no bits set
                tcpseg->setFinBit(false);
                tcpseg->setSynBit(false);
                tcpseg->setRstBit(false);
                tcpseg->setPshBit(false);
                tcpseg->setAckBit(false);
                tcpseg->setUrgBit(false);
                tcpseg->setEceBit(false);
                tcpseg->setCwrBit(false);
            }
            else{
                if(strstr(pktType, "FIN") != nullptr)
                    tcpseg->setFinBit(true);
                if(strstr(pktType, "SYN") != nullptr)
                    tcpseg->setSynBit(true);
                if(strstr(pktType, "RST") != nullptr)
                    tcpseg->setRstBit(true);
                if(strstr(pktType, "PSH") != nullptr)
                    tcpseg->setPshBit(true);
                if(strstr(pktType, "ACK") != nullptr)
                    tcpseg->setAckBit(true);
                if(strstr(pktType, "URG") != nullptr)
                    tcpseg->setUrgBit(true);
                if(strstr(pktType, "ECE") != nullptr)
                    tcpseg->setEceBit(true);
                if(strstr(pktType, "CWR") != nullptr)
                    tcpseg->setCwrBit(true);
            }
            tcpseg->setSrcPort(localPort);
            tcpseg->setDestPort(destPort);
            if (crcMode == CRC_COMPUTED) {
#ifdef WITH_IPv4
                auto ipv4 = dynamic_cast<INetfilter *>(getModuleByPath("^.ipv4.ip"));
                if (ipv4 != nullptr)
                    ipv4->registerHook(0, &tcpCrcInsertion);
#endif
#ifdef WITH_IPv6
                auto ipv6 = dynamic_cast<INetfilter *>(getModuleByPath("^.ipv6.ipv6"));
                if (ipv6 != nullptr)
                    ipv6->registerHook(0, &tcpCrcInsertion);
#endif
                tcpseg->setCrcMode(CRC_COMPUTED);
                auto pseudoHeader = makeShared<TransportPseudoHeader>();
                pseudoHeader->setSrcAddress(localAddr);
                pseudoHeader->setDestAddress(destAddr);
                pseudoHeader->setNetworkProtocolId(Protocol::ipv4.getId());
                pseudoHeader->setProtocolId(IP_PROT_TCP);
                pseudoHeader->setPacketLength(tcpseg->getChunkLength());
                // pseudoHeader length: ipv4: 12 bytes, ipv6: 40 bytes, other: ???
                pseudoHeader->setChunkLength(B(12));

                MemoryOutputStream stream;
                Chunk::serialize(stream, pseudoHeader);
                Chunk::serialize(stream, packetToSend->peekData(Chunk::PF_ALLOW_EMPTY));
                uint16_t crc = TcpIpChecksum::checksum(stream.getData());
                tcpseg->setCrc(crc);
                }
            else{
                tcpseg->setCrcMode(CRC_DECLARED_CORRECT);
                tcpseg->setCrc(0);
            }
            insertTransportProtocolHeader(packetToSend, Protocol::tcp, tcpseg);
        }
        else if(strstr(par("packetType"), "UDP")!= nullptr){ //UDP PACKET
            int socketId = getSimulation()->getUniqueNumber();
            ASSERT(destAddr.getType() == L3Address::IPv4);
            const auto& payload = makeShared<ApplicationPacket>();

            if(packetLength == -1)//default
                payload->setChunkLength(B(1472));//max packet length
            else
                payload->setChunkLength(B(packetLength));
            payload->setSequenceNumber(numSent++);
            payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
            payload->addTagIfAbsent<SocketInd>()->setSocketId(socketId);
            packetToSend->insertAtBack(payload);

            auto addressReq = packetToSend->addTagIfAbsent<L3AddressReq>();
            addressReq->setDestAddress(destAddr);
            if (destPort != -1)
                packetToSend->addTagIfAbsent<L4PortReq>()->setDestPort(destPort);
            auto& tags = getTags(packetToSend);
            tags.addTagIfAbsent<SocketReq>()->setSocketId(socketId);
            packetToSend->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);

            auto udpHeader = makeShared<UdpHeader>();
            udpHeader->setSourcePort(localPort);
            udpHeader->setDestinationPort(destPort);

            B totalLength = udpHeader->getChunkLength() + packetToSend->getTotalLength();
            if(totalLength.get() > 65535)
                throw cRuntimeError("send: total UDP message size exceeds %u", 65535);
            udpHeader->setTotalLengthField(totalLength);

            auto udpData = packetToSend->peekData(Chunk::PF_ALLOW_EMPTY);
            //CRC_COMPUTED(for packet capture)
            if (crcMode == CRC_COMPUTED) {
#ifdef WITH_IPv4
                auto ipv4 = dynamic_cast<INetfilter *>(getModuleByPath("^.ipv4.ip"));
                if (ipv4 != nullptr)
                    ipv4->registerHook(0, &udpCrcInsertion);
#endif
#ifdef WITH_IPv6
                auto ipv6 = dynamic_cast<INetfilter *>(getModuleByPath("^.ipv6.ipv6"));
                if (ipv6 != nullptr)
                    ipv6->registerHook(0, &udpCrcInsertion);
#endif
                udpHeader->setCrcMode(CRC_COMPUTED);
                auto crc = Udp::computeCrc(&Protocol::ipv4, localAddr, destAddr, udpHeader, udpData);
                udpHeader->setCrc(crc);
            }
            else{
                udpHeader->setCrcMode(CRC_DECLARED_CORRECT);
                udpHeader->setCrc(0);
            }
            insertTransportProtocolHeader(packetToSend, Protocol::udp, udpHeader);
        }
        else if(strstr(par("packetType"), "ICMP")!=nullptr){ //ICMP PACKET
            ASSERT(destAddr.getType() == L3Address::IPv4);
            auto payload = makeShared<ByteCountChunk>(B(1472));//Max length of payload
            const auto& request = makeShared<IcmpEchoRequest>();

            request->setIdentifier(getSimulation()->getUniqueNumber());
            request->setSeqNumber(numSent++);
            packetToSend->insertAtBack(payload);
            Icmp::insertCrc(CRC_COMPUTED, request, packetToSend);
            packetToSend->insertAtFront(request);
            packetToSend->addTag<PacketProtocolTag>()->setProtocol(&Protocol::icmpv4);

            auto addressReq = packetToSend->addTag<L3AddressReq>();
            addressReq->setSrcAddress(localAddr);
            addressReq->setDestAddress(destAddr);
            auto& tags = getTags(packetToSend);
            tags.addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);
            //tags.addTagIfAbsent<SocketReq>()->setSocketId(socketId);
        }
        else{
            throw cRuntimeError("PacketGenerator: Wrong Packet Type! Check your Parameter!\n");
        }
    }
}

void PacketGenerator::handleMessageWhenUp(cMessage *msg){
    sendPacket();
    simtime_t d = simTime() + par("sendInterval");
    if (stopTime < SIMTIME_ZERO || d < stopTime) {
        scheduleAt(d, selfMsg);
    }
    else {
        delete selfMsg;
        selfMsg = nullptr;
    }
}

void PacketGenerator::finish(){
    recordScalar("packets sent", numSent);
}

void PacketGenerator::handleStartOperation(LifecycleOperation *operation){
    simtime_t start = std::max(startTime, simTime());
    if ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime)) {
        scheduleAt(start, selfMsg);
    }
}

INetfilter::IHook::Result UdpCrcInsertion::datagramPostRoutingHook(Packet *packet)
{
    if (packet->findTag<InterfaceInd>())
        return ACCEPT;  // FORWARD

    auto networkProtocol = packet->getTag<PacketProtocolTag>()->getProtocol();
    const auto& networkHeader = getNetworkProtocolHeader(packet);
    if (networkHeader->getProtocol() == &Protocol::udp) {
        ASSERT(!networkHeader->isFragment());
        packet->eraseAtFront(networkHeader->getChunkLength());
        auto udpHeader = packet->removeAtFront<UdpHeader>();
        ASSERT(udpHeader->getCrcMode() == CRC_COMPUTED);
        const L3Address& srcAddress = networkHeader->getSourceAddress();
        const L3Address& destAddress = networkHeader->getDestinationAddress();
        Udp::insertCrc(networkProtocol, srcAddress, destAddress, udpHeader, packet);
        packet->insertAtFront(udpHeader);
        packet->insertAtFront(networkHeader);
    }

    return ACCEPT;
}

INetfilter::IHook::Result TcpCrcInsertion::datagramPostRoutingHook(Packet *packet)
{
    if (packet->findTag<InterfaceInd>())
        return ACCEPT;  // FORWARD
    auto networkProtocol = packet->getTag<PacketProtocolTag>()->getProtocol();
    const auto& networkHeader = getNetworkProtocolHeader(packet);
    if (networkHeader->getProtocol() == &Protocol::tcp) {
        ASSERT(!networkHeader->isFragment());
        packet->eraseAtFront(networkHeader->getChunkLength());
        auto tcpHeader = packet->removeAtFront<TcpHeader>();
        ASSERT(tcpHeader->getCrcMode() == CRC_COMPUTED);
        const L3Address& srcAddress = networkHeader->getSourceAddress();
        const L3Address& destAddress = networkHeader->getDestinationAddress();
        insertCrc(networkProtocol, srcAddress, destAddress, tcpHeader, packet);
        packet->insertAtFront(tcpHeader);
        packet->insertAtFront(networkHeader);
    }
    return ACCEPT;
}

void TcpCrcInsertion::insertCrc(const Protocol *networkProtocol, const L3Address& srcAddress, const L3Address& destAddress, const Ptr<TcpHeader>& tcpHeader, Packet *packet)
{
    auto crcMode = tcpHeader->getCrcMode();
    switch (crcMode) {
        case CRC_DECLARED_CORRECT:
            // if the CRC mode is declared to be correct, then set the CRC to an easily recognizable value
            tcpHeader->setCrc(0xC00D);
            break;
        case CRC_DECLARED_INCORRECT:
            // if the CRC mode is declared to be incorrect, then set the CRC to an easily recognizable value
            tcpHeader->setCrc(0xBAAD);
            break;
        case CRC_COMPUTED: {
            // if the CRC mode is computed, then compute the CRC and set it
            // this computation is delayed after the routing decision, see INetfilter hook
            tcpHeader->setCrc(0x0000); // make sure that the CRC is 0 in the TCP header before computing the CRC
            auto tcpData = packet->peekData(Chunk::PF_ALLOW_EMPTY);
            auto crc = computeCrc(networkProtocol, srcAddress, destAddress, tcpHeader, tcpData);
            tcpHeader->setCrc(crc);
            break;
        }
        default:
            throw cRuntimeError("Unknown CRC mode");
    }
}

uint16_t TcpCrcInsertion::computeCrc(const Protocol *networkProtocol, const L3Address& srcAddress, const L3Address& destAddress, const Ptr<const TcpHeader>& tcpHeader, const Ptr<const Chunk>& tcpData)
{
    auto pseudoHeader = makeShared<TransportPseudoHeader>();
    pseudoHeader->setSrcAddress(srcAddress);
    pseudoHeader->setDestAddress(destAddress);
    pseudoHeader->setNetworkProtocolId(networkProtocol->getId());
    pseudoHeader->setProtocolId(IP_PROT_TCP);
    pseudoHeader->setPacketLength(tcpHeader->getChunkLength() + tcpData->getChunkLength());
    // pseudoHeader length: ipv4: 12 bytes, ipv6: 40 bytes, generic: ???
    if (networkProtocol == &Protocol::ipv4)
        pseudoHeader->setChunkLength(B(12));
    else if (networkProtocol == &Protocol::ipv6)
        pseudoHeader->setChunkLength(B(40));
    else
        throw cRuntimeError("Unknown network protocol: %s", networkProtocol->getName());
    MemoryOutputStream stream;
    Chunk::serialize(stream, pseudoHeader);
    Chunk::serialize(stream, tcpHeader);
    Chunk::serialize(stream, tcpData);
    uint16_t crc = TcpIpChecksum::checksum(stream.getData());
    return crc;
}
