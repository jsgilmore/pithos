//
// Generated file, do not edit! Created by opp_msgc 4.1 from Pithos.msg.
//

#ifndef _PITHOS_M_H_
#define _PITHOS_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0401
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Enum generated from <tt>Pithos.msg</tt> by opp_msgc.
 * <pre>
 * enum PayloadTypes
 * {
 *     STORE_REQ = 1;
 *     WRITE = 2;
 * };
 * </pre>
 */
enum PayloadTypes {
    STORE_REQ = 1,
    WRITE = 2
};

/**
 * Class generated from <tt>Pithos.msg</tt> by opp_msgc.
 * <pre>
 * packet PithosMsg {
 *     int payloadType enum(PayloadTypes);
 * }
 * </pre>
 */
class PithosMsg : public ::cPacket
{
  protected:
    int payloadType_var;

    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PithosMsg&);

  public:
    PithosMsg(const char *name=NULL, int kind=0);
    PithosMsg(const PithosMsg& other);
    virtual ~PithosMsg();
    PithosMsg& operator=(const PithosMsg& other);
    virtual PithosMsg *dup() const {return new PithosMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getPayloadType() const;
    virtual void setPayloadType(int payloadType_var);
};

inline void doPacking(cCommBuffer *b, PithosMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PithosMsg& obj) {obj.parsimUnpack(b);}


#endif // _PITHOS_M_H_
