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

#include "Detector.h"

#include <inet/common/ModuleAccess.h>
#include <inet/common/packet/Packet.h>
#include <inet/common/packet/PacketFilter.h>
#include <inet/linklayer/ethernet/EtherPhyFrame_m.h>
#include <inet/linklayer/ethernet/EtherFrame_m.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>
#include "../ClientAgent/ClientAgent.h"
#include "../DetectAgent/DetectorMsgs_m.h"

using namespace inet;

Define_Module(Detector);

void Detector::initialize(int stage){
    if (stage == INITSTAGE_LOCAL) {
        if(strcmp(par("clientAgentModule"), "") != 0){
            clientAgent = getModuleFromPar<ClientAgent>(par("clientAgentModule"),this);
            connected = true;
            detectionTimeout = par("detectionTimeout");
        }
    }
}

void Detector::handleMessage(cMessage *msg){
    if(msg->isSelfMessage()){
        this->bubble("Detection Timeout");
        sendDirect(msg, clientAgent, "detectIn");
        return ;
    }
    //packet from
    EthernetSignal * signal = check_and_cast<EthernetSignal *>(msg);
    auto packet = check_and_cast<Packet *>(signal->decapsulate());
    delete msg;
    if(strstr(packet->getName(), "Flood") != nullptr){//detect condition here
        std::ostringstream str;
        str <<"DDoS Attack detected: " << packet->getName();
        this->bubble(str.str().c_str());
        if(connected){
            //send Detection
            const auto& phyHeader = packet->popAtFront<EthernetPhyHeader>();
            const auto& macHeader = packet->popAtFront<EthernetMacHeader>();
            const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();
            const auto& msg = makeShared<DetectorMsg>();
            Ipv4Address addr = ipv4Header->getDestAddress();

            msg->setChunkLength(B(addr.str().length()));
            msg->setAttackDetected(true);
            msg->setTargetPrefix(addr.str().c_str());
            Packet *p = new Packet("DDoS Detection");
            p->insertAtFront(msg);
            sendDirect(p, clientAgent, "detectIn");

            //schedule detection timeout
            for(int i = 0, n = pktList.size(); i < n; i++){ //check pktList
                Packet *tmp = pktList[i]->dup();
                const auto& m = tmp->peekAtFront<DetectorMsg>();
                if(!strcmp(m->getTargetPrefix(), addr.str().c_str())){
                    cancelAndDelete(pktList[i]);
                    pktList.erase(pktList.begin()+i);
                    pktList.push_back(tmp);
                    scheduleAt(simTime()+detectionTimeout, tmp);
                    delete packet;
                    return ;
                }
                delete tmp;
            }
            const auto& timeout = makeShared<DetectorMsg>();
            timeout->setChunkLength(B(addr.str().length()));
            timeout->setAttackDetected(false);
            timeout->setTargetPrefix(addr.str().c_str());
            Packet *pk = new Packet("Detection Timeout");
            pk->insertAtFront(timeout);
            pktList.push_back(pk);
            scheduleAt(simTime()+detectionTimeout, pk);
        }
    }
    delete packet;
}
