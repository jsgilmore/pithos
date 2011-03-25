//
// Generated file, do not edit! Created by opp_msgc 4.1 from Pithos.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Pithos_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("PayloadTypes");
    if (!e) enums.getInstance()->add(e = new cEnum("PayloadTypes"));
    e->insert(WRITE, "WRITE");
    e->insert(OVERLAY_WRITE, "OVERLAY_WRITE");
    e->insert(INFORM, "INFORM");
    e->insert(INFORM_REQ, "INFORM_REQ");
);

Register_Class(PithosMsg);

PithosMsg::PithosMsg(const char *name, int kind) : cPacket(name,kind)
{
    this->value_var = 0;
    this->payloadType_var = 0;
}

PithosMsg::PithosMsg(const PithosMsg& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

PithosMsg::~PithosMsg()
{
}

PithosMsg& PithosMsg::operator=(const PithosMsg& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->value_var = other.value_var;
    this->payloadType_var = other.payloadType_var;
    return *this;
}

void PithosMsg::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->value_var);
    doPacking(b,this->payloadType_var);
}

void PithosMsg::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->value_var);
    doUnpacking(b,this->payloadType_var);
}

int PithosMsg::getValue() const
{
    return value_var;
}

void PithosMsg::setValue(int value_var)
{
    this->value_var = value_var;
}

int PithosMsg::getPayloadType() const
{
    return payloadType_var;
}

void PithosMsg::setPayloadType(int payloadType_var)
{
    this->payloadType_var = payloadType_var;
}

class PithosMsgDescriptor : public cClassDescriptor
{
  public:
    PithosMsgDescriptor();
    virtual ~PithosMsgDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PithosMsgDescriptor);

PithosMsgDescriptor::PithosMsgDescriptor() : cClassDescriptor("PithosMsg", "cPacket")
{
}

PithosMsgDescriptor::~PithosMsgDescriptor()
{
}

bool PithosMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PithosMsg *>(obj)!=NULL;
}

const char *PithosMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PithosMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int PithosMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *PithosMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "value",
        "payloadType",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int PithosMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "value")==0) return base+0;
    if (fieldName[0]=='p' && strcmp(fieldName, "payloadType")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PithosMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *PithosMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 1:
            if (!strcmp(propertyname,"enum")) return "PayloadTypes";
            return NULL;
        default: return NULL;
    }
}

int PithosMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PithosMsg *pp = (PithosMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PithosMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PithosMsg *pp = (PithosMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getValue());
        case 1: return long2string(pp->getPayloadType());
        default: return "";
    }
}

bool PithosMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PithosMsg *pp = (PithosMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setValue(string2long(value)); return true;
        case 1: pp->setPayloadType(string2long(value)); return true;
        default: return false;
    }
}

const char *PithosMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *PithosMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PithosMsg *pp = (PithosMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


