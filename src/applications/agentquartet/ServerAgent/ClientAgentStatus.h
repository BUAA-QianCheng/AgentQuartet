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

#ifndef APPLICATIONS_AGENTQUARTET_SERVERAGENT_CLIENTAGENTSTATUS_H_
#define APPLICATIONS_AGENTQUARTET_SERVERAGENT_CLIENTAGENTSTATUS_H_

#include <map>
#include <vector>
#include <inet/common/INETDefs.h>
#include "../common/AQCommon.h"

using namespace inet;

class ClientAgentStatus : public cObject{
  protected:
    std::string cuid;
    typedef std::map<std::string , MitigationStatistics *> Mitigations; //a map for mid - MitigationStatistics
    Mitigations mitigations;
    typedef std::vector<Acl> Acls;
    Acls acls;

  public:
    virtual std::string str() const override;
    virtual bool sameTarget(MitigationStatistics *m1, MitigationStatistics *m2);

    //Cuid
    virtual void setCuid(std::string s){ this->cuid = s; }
    virtual std::string getCuid() { return this->cuid; }

    //Mitigation
    virtual MitigationStatistics *createToStatistics(const inet::Ptr<const MitigationCreate> &c);
    virtual bool createMitigations(std::string mid, MitigationStatistics * m);
    virtual MitigationStatistics *getMitigations(std::string mid);
    virtual MitigationStatistics *deleteMitigations(std::string mid);
    virtual void setMitigationStatus(std::string mid, MitigationStatus status);

    //Acl
    virtual bool createAcl(Acl* acl);
    virtual Acl *getAcl(std::string name);
    virtual void deleteAcl(std::string name);

};

#endif /* APPLICATIONS_AGENTQUARTET_SERVERAGENT_CLIENTAGENTSTATUS_H_ */
