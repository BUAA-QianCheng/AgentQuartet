//
// Generated file, do not edit! Created by nedtool 5.6 from applications/agentquartet/RouterAgent/RouterAgentMsgs.msg.
//

#ifndef __INET_ROUTERAGENTMSGS_M_H
#define __INET_ROUTERAGENTMSGS_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif


namespace inet {

class RouterAgentMsg;
} // namespace inet

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk


namespace inet {

/**
 * Enum generated from <tt>applications/agentquartet/RouterAgent/RouterAgentMsgs.msg:20</tt> by nedtool.
 * <pre>
 * enum RouterMsgType
 * {
 *     CREATE_FILTERRULE = 1;
 *     DELETE_FILTERRULE = 2;
 *     CREATE_ROUTE = 3;
 *     CHANGE_ROUTE = 4;
 *     DELETE_ROUTE = 5;
 *     TO_IDLE_FILTER_TYPE = 6;
 *     TO_DROPLIST_ONLY_FILTERTYPE = 7;
 *     TO_ACCEPTLIST_ONLY_FILTERTYPE = 8;
 * }
 * </pre>
 */
enum RouterMsgType {
    CREATE_FILTERRULE = 1,
    DELETE_FILTERRULE = 2,
    CREATE_ROUTE = 3,
    CHANGE_ROUTE = 4,
    DELETE_ROUTE = 5,
    TO_IDLE_FILTER_TYPE = 6,
    TO_DROPLIST_ONLY_FILTERTYPE = 7,
    TO_ACCEPTLIST_ONLY_FILTERTYPE = 8
};

/**
 * Class generated from <tt>applications/agentquartet/RouterAgent/RouterAgentMsgs.msg:31</tt> by nedtool.
 * <pre>
 * class RouterAgentMsg extends FieldsChunk
 * {
 *     RouterMsgType type;
 *     string strs[];
 * }
 * </pre>
 */
class RouterAgentMsg : public ::inet::FieldsChunk
{
  protected:
    inet::RouterMsgType type = static_cast<inet::RouterMsgType>(-1);
    omnetpp::opp_string *strs = nullptr;
    size_t strs_arraysize = 0;

  private:
    void copy(const RouterAgentMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const RouterAgentMsg&);

  public:
    RouterAgentMsg();
    RouterAgentMsg(const RouterAgentMsg& other);
    virtual ~RouterAgentMsg();
    RouterAgentMsg& operator=(const RouterAgentMsg& other);
    virtual RouterAgentMsg *dup() const override {return new RouterAgentMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual inet::RouterMsgType getType() const;
    virtual void setType(inet::RouterMsgType type);
    virtual void setStrsArraySize(size_t size);
    virtual size_t getStrsArraySize() const;
    virtual const char * getStrs(size_t k) const;
    virtual void setStrs(size_t k, const char * strs);
    virtual void insertStrs(const char * strs);
    virtual void insertStrs(size_t k, const char * strs);
    virtual void eraseStrs(size_t k);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const RouterAgentMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, RouterAgentMsg& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_ROUTERAGENTMSGS_M_H

