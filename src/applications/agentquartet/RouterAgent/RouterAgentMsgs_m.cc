//
// Generated file, do not edit! Created by nedtool 5.6 from applications/agentquartet/RouterAgent/RouterAgentMsgs.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include "RouterAgentMsgs_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace {
template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)(static_cast<const omnetpp::cObject *>(t));
}

template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)dynamic_cast<const void *>(t);
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)static_cast<const void *>(t);
}

}

namespace inet {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule to generate operator<< for shared_ptr<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');

    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::RouterMsgType");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::RouterMsgType"));
    e->insert(CREATE_FILTERRULE, "CREATE_FILTERRULE");
    e->insert(DELETE_FILTERRULE, "DELETE_FILTERRULE");
    e->insert(CREATE_ROUTE, "CREATE_ROUTE");
    e->insert(CHANGE_ROUTE, "CHANGE_ROUTE");
    e->insert(DELETE_ROUTE, "DELETE_ROUTE");
    e->insert(TO_IDLE_FILTER_TYPE, "TO_IDLE_FILTER_TYPE");
    e->insert(TO_DROPLIST_ONLY_FILTERTYPE, "TO_DROPLIST_ONLY_FILTERTYPE");
    e->insert(TO_ACCEPTLIST_ONLY_FILTERTYPE, "TO_ACCEPTLIST_ONLY_FILTERTYPE");
)

Register_Class(RouterAgentMsg)

RouterAgentMsg::RouterAgentMsg() : ::inet::FieldsChunk()
{
}

RouterAgentMsg::RouterAgentMsg(const RouterAgentMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

RouterAgentMsg::~RouterAgentMsg()
{
    delete [] this->strs;
}

RouterAgentMsg& RouterAgentMsg::operator=(const RouterAgentMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void RouterAgentMsg::copy(const RouterAgentMsg& other)
{
    this->type = other.type;
    delete [] this->strs;
    this->strs = (other.strs_arraysize==0) ? nullptr : new omnetpp::opp_string[other.strs_arraysize];
    strs_arraysize = other.strs_arraysize;
    for (size_t i = 0; i < strs_arraysize; i++) {
        this->strs[i] = other.strs[i];
    }
}

void RouterAgentMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->type);
    b->pack(strs_arraysize);
    doParsimArrayPacking(b,this->strs,strs_arraysize);
}

void RouterAgentMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->type);
    delete [] this->strs;
    b->unpack(strs_arraysize);
    if (strs_arraysize == 0) {
        this->strs = nullptr;
    } else {
        this->strs = new omnetpp::opp_string[strs_arraysize];
        doParsimArrayUnpacking(b,this->strs,strs_arraysize);
    }
}

inet::RouterMsgType RouterAgentMsg::getType() const
{
    return this->type;
}

void RouterAgentMsg::setType(inet::RouterMsgType type)
{
    handleChange();
    this->type = type;
}

size_t RouterAgentMsg::getStrsArraySize() const
{
    return strs_arraysize;
}

const char * RouterAgentMsg::getStrs(size_t k) const
{
    if (k >= strs_arraysize) throw omnetpp::cRuntimeError("Array of size strs_arraysize indexed by %lu", (unsigned long)k);
    return this->strs[k].c_str();
}

void RouterAgentMsg::setStrsArraySize(size_t newSize)
{
    handleChange();
    omnetpp::opp_string *strs2 = (newSize==0) ? nullptr : new omnetpp::opp_string[newSize];
    size_t minSize = strs_arraysize < newSize ? strs_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        strs2[i] = this->strs[i];
    delete [] this->strs;
    this->strs = strs2;
    strs_arraysize = newSize;
}

void RouterAgentMsg::setStrs(size_t k, const char * strs)
{
    if (k >= strs_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    this->strs[k] = strs;
}

void RouterAgentMsg::insertStrs(size_t k, const char * strs)
{
    handleChange();
    if (k > strs_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    size_t newSize = strs_arraysize + 1;
    omnetpp::opp_string *strs2 = new omnetpp::opp_string[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        strs2[i] = this->strs[i];
    strs2[k] = strs;
    for (i = k + 1; i < newSize; i++)
        strs2[i] = this->strs[i-1];
    delete [] this->strs;
    this->strs = strs2;
    strs_arraysize = newSize;
}

void RouterAgentMsg::insertStrs(const char * strs)
{
    insertStrs(strs_arraysize, strs);
}

void RouterAgentMsg::eraseStrs(size_t k)
{
    if (k >= strs_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    size_t newSize = strs_arraysize - 1;
    omnetpp::opp_string *strs2 = (newSize == 0) ? nullptr : new omnetpp::opp_string[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        strs2[i] = this->strs[i];
    for (i = k; i < newSize; i++)
        strs2[i] = this->strs[i+1];
    delete [] this->strs;
    this->strs = strs2;
    strs_arraysize = newSize;
}

class RouterAgentMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_type,
        FIELD_strs,
    };
  public:
    RouterAgentMsgDescriptor();
    virtual ~RouterAgentMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(RouterAgentMsgDescriptor)

RouterAgentMsgDescriptor::RouterAgentMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::RouterAgentMsg)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

RouterAgentMsgDescriptor::~RouterAgentMsgDescriptor()
{
    delete[] propertynames;
}

bool RouterAgentMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RouterAgentMsg *>(obj)!=nullptr;
}

const char **RouterAgentMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RouterAgentMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RouterAgentMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int RouterAgentMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        0,    // FIELD_type
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_strs
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *RouterAgentMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "type",
        "strs",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int RouterAgentMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 't' && strcmp(fieldName, "type") == 0) return base+0;
    if (fieldName[0] == 's' && strcmp(fieldName, "strs") == 0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RouterAgentMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "inet::RouterMsgType",    // FIELD_type
        "string",    // FIELD_strs
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **RouterAgentMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_type: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *RouterAgentMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_type:
            if (!strcmp(propertyname, "enum")) return "inet::RouterMsgType";
            return nullptr;
        default: return nullptr;
    }
}

int RouterAgentMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RouterAgentMsg *pp = (RouterAgentMsg *)object; (void)pp;
    switch (field) {
        case FIELD_strs: return pp->getStrsArraySize();
        default: return 0;
    }
}

const char *RouterAgentMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RouterAgentMsg *pp = (RouterAgentMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RouterAgentMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RouterAgentMsg *pp = (RouterAgentMsg *)object; (void)pp;
    switch (field) {
        case FIELD_type: return enum2string(pp->getType(), "inet::RouterMsgType");
        case FIELD_strs: return oppstring2string(pp->getStrs(i));
        default: return "";
    }
}

bool RouterAgentMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RouterAgentMsg *pp = (RouterAgentMsg *)object; (void)pp;
    switch (field) {
        case FIELD_strs: pp->setStrs(i,(value)); return true;
        default: return false;
    }
}

const char *RouterAgentMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *RouterAgentMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RouterAgentMsg *pp = (RouterAgentMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace inet

