//
// Generated file, do not edit! Created by nedtool 5.6 from apps/agentquartet/DetectAgent/DetectorMsgs.msg.
//

#ifndef __DETECTORMSGS_M_H
#define __DETECTORMSGS_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



class DetectorMsg;
#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

/**
 * Class generated from <tt>apps/agentquartet/DetectAgent/DetectorMsgs.msg:19</tt> by nedtool.
 * <pre>
 * class DetectorMsg extends inet::FieldsChunk
 * {
 *     bool attackDetected;
 *     string targetPrefix;
 * }
 * </pre>
 */
class DetectorMsg : public ::inet::FieldsChunk
{
  protected:
    bool attackDetected = false;
    omnetpp::opp_string targetPrefix;

  private:
    void copy(const DetectorMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const DetectorMsg&);

  public:
    DetectorMsg();
    DetectorMsg(const DetectorMsg& other);
    virtual ~DetectorMsg();
    DetectorMsg& operator=(const DetectorMsg& other);
    virtual DetectorMsg *dup() const override {return new DetectorMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual bool getAttackDetected() const;
    virtual void setAttackDetected(bool attackDetected);
    virtual const char * getTargetPrefix() const;
    virtual void setTargetPrefix(const char * targetPrefix);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const DetectorMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, DetectorMsg& obj) {obj.parsimUnpack(b);}

#endif // ifndef __DETECTORMSGS_M_H

