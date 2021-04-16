/*
 * AreaInfo.h
 *
 *  Created on: 2021Äê3ÔÂ16ÈÕ
 *      Author: QianCheng
 */

#ifndef APPLICATIONS_AGENTQUARTET_SERVERAGENT_AREAINFO_H_
#define APPLICATIONS_AGENTQUARTET_SERVERAGENT_AREAINFO_H_

#include <inet/common/INETDefs.h>
#include <inet/networklayer/common/L3AddressResolver.h>

using namespace inet;

/*
 *Config Example: <config> <areainfo address= '10.0.0.0' prefixLegnth = '24' router='router1' /> </config>
 */
class AreaInfo : public cObject {
  protected:
    L3Address address, router;
    int prefixLength;

  public:
    //gets and sets
    virtual L3Address getAddress(){ return address; }
    virtual void setAddress(L3Address address) { this->address = address; }
    virtual L3Address getRouter() { return router; }
    virtual void setRouter(L3Address router) { this->router = router; }
    virtual int getPrefixLength(){ return prefixLength; }
    virtual void setPrefixLength(int length) { this->prefixLength = length; }
    virtual std::string str() const override { std::stringstream out; out << address.toIpv4().str() << "/" << prefixLength <<" <- "<< router.str(); return out.str();  }
};

Register_Class(AreaInfo);

#endif /* APPLICATIONS_AGENTQUARTET_SERVERAGENT_AREAINFO_H_ */
