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

#include "FilterRuleTable.h"

#include <algorithm>
#include <sstream>

#include <inet/common/INETUtils.h>
#include <inet/common/ModuleAccess.h>
#include <inet/common/PatternMatcher.h>
#include <inet/common/Simsignals.h>
#include <inet/common/XMLUtils.h>

using namespace inet;
using namespace utils;

Define_Module(FilterRuleTable);

std::ostream& operator<<(std::ostream& os, const FilterRule& fr)
{
    os << fr.str();
    return os;
};

void FilterRuleTable::initialize(int stage){
    using namespace xmlutils;
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        config = par("config");
        cXMLElementList ruleElements = config->getChildrenByTagName("filterrule");
        for(auto & ruleElement : ruleElements){
            try{
                const char *rule = ruleElement->getAttribute("content");
                if(rule[0] != 0){
                    FilterRule *tmpRule = new FilterRule(std::string(rule));
                    this->addFilterRule(tmpRule);
                }
            }catch(std::exception e){
                throw cRuntimeError("Error in XML <FilterRule> element at %s: %s", ruleElement->getSourceLocation(), e.what());
            }
        }
        WATCH_PTRVECTOR(rules);
        WATCH(filterType);
    }
}

void FilterRuleTable::handleMessage(cMessage *msg){
    throw cRuntimeError("This module doesn't process messages");
}

void FilterRuleTable::refreshDisplay() const {
    char buf[80];
    sprintf(buf, "%d rules", (int)rules.size());
    getDisplayString().setTagArg("t", 0, buf);
}

void FilterRuleTable::addFilterRule(FilterRule *entry){
    entry->setFilterRuleTable(this);
    if(entry->getAction() == D)
        this->rules.insert(rules.begin(), 1, entry);
    else
        this->rules.push_back(entry);
}

int FilterRuleTable::matchPacket(Packet * packet){
    Enter_Method("Match Packet");
    for (auto rule : rules){
        Packet *p = packet->dup();
        if(rule->matchPacket(p)){
            rule->addPktsDropped(1);
            rule->addOctetsDropped(p->getByteLength());
            delete p;
            std::vector<FilterRule *>::iterator it = find(rules.begin(), rules.end(), rule);
            return std::distance(rules.begin(), it);
        }
        delete p;
    }
    return -1;
}

bool FilterRuleTable::inTable(FilterRule* rule){
    for(FilterRule* r : rules){
        if(!strcmp(r->str().c_str(), rule->str().c_str()))
            return true;
    }
    return false;
}

FilterRule* FilterRuleTable::getFilterRule(int index){
    if(index < 0 || index > rules.size())
        return nullptr;
    else
        return rules[index];
}

