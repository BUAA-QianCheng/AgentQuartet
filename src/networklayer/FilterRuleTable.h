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

#ifndef NETWORKLAYER_FILTERRULETABLE_H_
#define NETWORKLAYER_FILTERRULETABLE_H_

#include "networklayer/FilterRule.h"

#include <vector>
#include <algorithm>

#include <inet/common/INETDefs.h>
#include <inet/common/INETMath.h>
#include <inet/common/lifecycle/ILifecycle.h>
#include <inet/networklayer/contract/ipv4/Ipv4Address.h>
#include <inet/common/packet/Packet.h>

using namespace inet;

inline bool isEmpty(const char *s) { return !s || !s[0]; }

class FilterRule;

enum FilterType{
    IDLE = 0,//Forward All Packets
    DROPLIST_ONLY =1, //Forward if not Match DropList
    ACCEPTLIST_ONLY =2 //Match DropList First, Forward only if Packet Match a Accept Rule.
};

class FilterRuleTable : public cSimpleModule{
  private:
    FilterType filterType = DROPLIST_ONLY;
    typedef std::vector<FilterRule *> FilterRules;
    FilterRules rules;
    cXMLElement *config = nullptr;

  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

  public:
    virtual void addFilterRule(FilterRule * entry);
    virtual FilterRule* getFilterRule(int index);
    virtual int matchPacket(Packet * packet);
    virtual bool inTable(FilterRule* rule);

    virtual void setFilterType(FilterType type){ this->filterType = type; }
    virtual FilterType getFilterType(){ return this->filterType; }

};

#endif /* NETWORKLAYER_FILTERRULETABLE_H_ */
