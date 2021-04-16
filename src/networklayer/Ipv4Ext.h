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

#ifndef NETWORKLAYER_IPV4EXT_H_
#define NETWORKLAYER_IPV4EXT_H_

#include "FilterRuleTable.h"
#include "applications/agentquartet/RouterAgent/RouterAgent.h"

#include <inet/networklayer/ipv4/Ipv4.h>
#include <inet/common/packet/Packet.h>

using namespace inet;

class Ipv4Ext : public Ipv4{
  protected:
    FilterRuleTable *frt = nullptr;
    RouterAgent * ra = nullptr;
  protected:
    virtual void initialize(int stage) override;
    virtual void preroutingFinish(Packet *packet) override;
    virtual void FilterPacket(Packet * packet, int result);

};

#endif /* NETWORKLAYER_IPV4EXT_H_ */
