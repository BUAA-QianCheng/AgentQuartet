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

#ifndef APPLICATIONS_AGENTQUARTET_CLIENTAGENT_MITIGATIONINFO_H_
#define APPLICATIONS_AGENTQUARTET_CLIENTAGENT_MITIGATIONINFO_H_

#include <inet/common/INETDefs.h>

using namespace inet;

class MitigationInfo : public cObject{
  private:
    std::string mid;
    typedef std::vector<std::string> Targets;
    Targets targets;
    simtime_t createTime;
  public:
    virtual std::string str() const override;

  //gets and sets
    virtual void setMid(std::string s){ this->mid = s; }
    virtual std::string getMid(){ return this->mid; }
    virtual void setCreateTime(simtime_t t) { this->createTime = t; }
    virtual simtime_t getCreateTime() { return createTime; }

    virtual void addTarget(std::string s) { targets.push_back(s); }
    virtual bool inTargets(std::string s);
};

#endif /* APPLICATIONS_AGENTQUARTET_CLIENTAGENT_MITIGATIONINFO_H_ */
