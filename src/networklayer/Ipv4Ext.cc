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

#include "Ipv4Ext.h"

#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/networklayer/common/NextHopAddressTag_m.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "networklayer/FilterRuleTable.h"

using namespace inet;

Define_Module(Ipv4Ext);

void Ipv4Ext::initialize(int stage){
    OperationalBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        frt = getModuleFromPar<FilterRuleTable>(par("filterRuleTableModule"),this);
        if( par("hasAQ") ){
            ra = getModuleFromPar<RouterAgent>(par("routerAgentModule"),this);
        }

        ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        rt = getModuleFromPar<IIpv4RoutingTable>(par("routingTableModule"), this);
        arp = getModuleFromPar<IArp>(par("arpModule"), this);
        icmp = getModuleFromPar<Icmp>(par("icmpModule"), this);

        transportInGateBaseId = gateBaseId("transportIn");

        const char *crcModeString = par("crcMode");
        crcMode = parseCrcMode(crcModeString, false);

        defaultTimeToLive = par("timeToLive");
        defaultMCTimeToLive = par("multicastTimeToLive");
        fragmentTimeoutTime = par("fragmentTimeout");
        limitedBroadcast = par("limitedBroadcast");
        directBroadcastInterfaces = par("directBroadcastInterfaces").stdstringValue();

        directBroadcastInterfaceMatcher.setPattern(directBroadcastInterfaces.c_str(), false, true, false);

        curFragmentId = 0;
        lastCheckTime = 0;

        numMulticast = numLocalDeliver = numDropped = numUnroutable = numForwarded = 0;

        // NetFilter:
        hooks.clear();
        queuedDatagramsForHooks.clear();

        pendingPackets.clear();
        cModule *arpModule = check_and_cast<cModule *>(arp);
        arpModule->subscribe(IArp::arpResolutionCompletedSignal, this);
        arpModule->subscribe(IArp::arpResolutionFailedSignal, this);

        registerService(Protocol::ipv4, gate("transportIn"), gate("queueIn"));
        registerProtocol(Protocol::ipv4, gate("queueOut"), gate("transportOut"));

        WATCH(numMulticast);
        WATCH(numLocalDeliver);
        WATCH(numDropped);
        WATCH(numUnroutable);
        WATCH(numForwarded);
        WATCH_MAP(pendingPackets);
        WATCH_MAP(socketIdToSocketDescriptor);
    }
}

void Ipv4Ext::FilterPacket(Packet * packet, int result){
    stringstream out;
    if(frt->getFilterRule(result) != nullptr)
        out << "Rule Matched: " << frt->getFilterRule(result)->str();
    else
        out << "No Accept Rule for Packet" ;
    bubble(out.str().c_str());
    numDropped++;
    PacketDropDetails details;
    details.setReason(OTHER_PACKET_DROP);
    emit(packetDroppedSignal, packet, &details);
    delete packet;
}

void Ipv4Ext::preroutingFinish(Packet *packet)
{
    const InterfaceEntry *fromIE = ift->getInterfaceById(packet->getTag<InterfaceInd>()->getInterfaceId());
    Ipv4Address nextHopAddr = getNextHop(packet);

    if(frt->getFilterType() != IDLE){
        int result = frt->matchPacket(packet);
        if(result == -1 && frt->getFilterType() == ACCEPTLIST_ONLY){
            if(par("hasAQ")){
                FilterRule *r = new FilterRule();
                r->genFilterRuleFromPacket(packet, D);
                if(!frt->inTable(r) && strstr(packet->getName(), "Flood") != nullptr){
                    ra->sendFilterRuleToServerAgent(r->str());
                }
            }
            FilterPacket(packet, result);
            return ;
        }
        else if(result != -1 && frt->getFilterRule(result)->getAction() == D){
            FilterPacket(packet, result);
            return ;
        }
    }

    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();
    ASSERT(ipv4Header);
    Ipv4Address destAddr = ipv4Header->getDestAddress();

    // route packet

    if (fromIE->isLoopback()) {
        reassembleAndDeliver(packet);
    }
    else if (destAddr.isMulticast()) {
        // check for local delivery
        // Note: multicast routers will receive IGMP datagrams even if their interface is not joined to the group
        if (fromIE->getProtocolData<Ipv4InterfaceData>()->isMemberOfMulticastGroup(destAddr) ||
            (rt->isMulticastForwardingEnabled() && ipv4Header->getProtocolId() == IP_PROT_IGMP))
            reassembleAndDeliver(packet->dup());
        else
            EV_WARN << "Skip local delivery of multicast datagram (input interface not in multicast group)\n";

        // don't forward if IP forwarding is off, or if dest address is link-scope
        if (!rt->isMulticastForwardingEnabled()) {
            EV_WARN << "Skip forwarding of multicast datagram (forwarding disabled)\n";
            delete packet;
        }
        else if (destAddr.isLinkLocalMulticast()) {
            EV_WARN << "Skip forwarding of multicast datagram (packet is link-local)\n";
            delete packet;
        }
        else if (ipv4Header->getTimeToLive() <= 1) {      // TTL before decrement
            EV_WARN << "Skip forwarding of multicast datagram (TTL reached 0)\n";
            delete packet;
        }
        else
            forwardMulticastPacket(prepareForForwarding(packet));
    }
    else {
        const InterfaceEntry *broadcastIE = nullptr;

        // check for local delivery; we must accept also packets coming from the interfaces that
        // do not yet have an IP address assigned. This happens during DHCP requests.
        if (rt->isLocalAddress(destAddr) || fromIE->getProtocolData<Ipv4InterfaceData>()->getIPAddress().isUnspecified()) {
            reassembleAndDeliver(packet);
        }
        else if (destAddr.isLimitedBroadcastAddress() || (broadcastIE = rt->findInterfaceByLocalBroadcastAddress(destAddr))) {
            // broadcast datagram on the target subnet if we are a router
            if (broadcastIE && fromIE != broadcastIE && rt->isForwardingEnabled()) {
                if (directBroadcastInterfaceMatcher.matches(broadcastIE->getInterfaceName()) ||
                    directBroadcastInterfaceMatcher.matches(broadcastIE->getInterfaceFullPath().c_str()))
                {
                    auto packetCopy = prepareForForwarding(packet->dup());
                    packetCopy->addTagIfAbsent<InterfaceReq>()->setInterfaceId(broadcastIE->getInterfaceId());
                    packetCopy->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(Ipv4Address::ALLONES_ADDRESS);
                    fragmentPostRouting(packetCopy);
                }
                else
                    EV_INFO << "Forwarding of direct broadcast packets is disabled on interface " << broadcastIE->getInterfaceName() << std::endl;
            }

            EV_INFO << "Broadcast received\n";
            reassembleAndDeliver(packet);
        }
        else if (!rt->isForwardingEnabled()) {
            EV_WARN << "forwarding off, dropping packet\n";
            numDropped++;
            PacketDropDetails details;
            details.setReason(FORWARDING_DISABLED);
            emit(packetDroppedSignal, packet, &details);
            delete packet;
        }
        else {
            packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(nextHopAddr);
            routeUnicastPacket(prepareForForwarding(packet));
        }
    }
}

