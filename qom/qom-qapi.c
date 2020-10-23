#include "qemu/osdep.h"
#include "qom/qom-qapi.h"
#include "qapi/visitor.h"

/**
 * QOMVisitorWrapper: a visitor wrapper
 * @v: visitor object
 * @name: name argument for visitor
 * @obj: pointer to area containing the value
 * @size: size of area containing value
 * @errp: pointer to error information
 *
 * A visitor wrapper is a function that will get a pointer to
 * a memory area, and call a corresponding QAPI visitor
 * passing that memory area as argument.
 */
typedef bool QOMVisitorWrapper(Visitor *v, const char *name,
                               void *obj, size_t size, Error **errp);

/**
 * struct QOMVisitorType: info about a given visitor type
 */
struct QOMVisitorType {
    /** data type name */
    const char *type_name;
    /** Expected size of memory area holding type */
    size_t size;

    /** Visitor wrapper function */
    QOMVisitorWrapper *visit;
};

#define stringify(s) tostring(s)
#define tostring(s) #s

#define DEFINE_VISITOR_TYPE(name, qapi_visitor)                                \
    static bool visitor_wrapper_##name(Visitor *v, const char *_name,          \
                                      void *obj, size_t size, Error **errp)    \
    {                                                                          \
        QOM_VISITOR_C_TYPE(name) *ptr = obj;                                   \
        assert(size == sizeof(QOM_VISITOR_C_TYPE(name)));                      \
        return qapi_visitor(v, _name, ptr, errp);                              \
    }                                                                          \
    const QOMVisitorType qom_visitor_##name ={                                 \
        .type_name = stringify(name),                                          \
        .size = sizeof(QOM_VISITOR_C_TYPE(name)),                              \
        .visit = visitor_wrapper_##name,                                       \
    };

DEFINE_VISITOR_TYPE(int8, visit_type_int8);
DEFINE_VISITOR_TYPE(int16, visit_type_int16);
DEFINE_VISITOR_TYPE(int32, visit_type_int32);
DEFINE_VISITOR_TYPE(int64, visit_type_int64);

DEFINE_VISITOR_TYPE(uint8, visit_type_uint8);
DEFINE_VISITOR_TYPE(uint16, visit_type_uint16);
DEFINE_VISITOR_TYPE(uint32, visit_type_uint32);
DEFINE_VISITOR_TYPE(uint64, visit_type_uint64);

DEFINE_VISITOR_TYPE(bool, visit_type_bool);
DEFINE_VISITOR_TYPE(str, visit_type_str);

DEFINE_VISITOR_TYPE(size, visit_type_size);


bool qom_visitor_call(const QOMVisitorType *visitor_type,
                      Visitor *v, const char *name,
                      void *obj, size_t size, Error **errp)
{
    return visitor_type->visit(v, name, obj, size, errp);
}

const char *qom_visitor_type_name(const QOMVisitorType *visitor_type)
{
    return visitor_type->type_name;
}
