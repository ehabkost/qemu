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
 * struct QAPITypeInfo: info about a given visitor type
 */
struct QAPITypeInfo {
    /** data type name */
    const char *type_name;
    /** Expected size of memory area holding type */
    size_t size;

    /** Visitor wrapper function */
    QOMVisitorWrapper *visit;
};

#define stringify(s) tostring(s)
#define tostring(s) #s

#define DEFINE_QAPI_TYPE(var_name, qapi_visitor)                               \
    static bool visitor_wrapper_##var_name(Visitor *v, const char *_name,      \
                                      void *obj, size_t size, Error **errp)    \
    {                                                                          \
        QOM_QAPI_C_TYPE(var_name) *ptr = obj;                                  \
        assert(size == sizeof(QOM_QAPI_C_TYPE(var_name)));                     \
        return qapi_visitor(v, _name, ptr, errp);                              \
    }                                                                          \
    const QAPITypeInfo var_name = {                                            \
        .type_name = stringify(var_name),                                      \
        .size = sizeof(QOM_QAPI_C_TYPE(var_name)),                             \
        .visit = visitor_wrapper_##var_name,                                   \
    };

DEFINE_QAPI_TYPE(qapi_int8, visit_type_int8);
DEFINE_QAPI_TYPE(qapi_int16, visit_type_int16);
DEFINE_QAPI_TYPE(qapi_int32, visit_type_int32);
DEFINE_QAPI_TYPE(qapi_int64, visit_type_int64);

DEFINE_QAPI_TYPE(qapi_uint8, visit_type_uint8);
DEFINE_QAPI_TYPE(qapi_uint16, visit_type_uint16);
DEFINE_QAPI_TYPE(qapi_uint32, visit_type_uint32);
DEFINE_QAPI_TYPE(qapi_uint64, visit_type_uint64);

DEFINE_QAPI_TYPE(qapi_bool, visit_type_bool);
DEFINE_QAPI_TYPE(qapi_str, visit_type_str);

DEFINE_QAPI_TYPE(qapi_size, visit_type_size);


bool qom_qapi_visitor_call(const QAPITypeInfo *qapi_type,
                           Visitor *v, const char *name,
                           void *obj, size_t size, Error **errp)
{
    return qapi_type->visit(v, name, obj, size, errp);
}

const char *qom_qapi_type_name(const QAPITypeInfo *qapi_type)
{
    return qapi_type->type_name;
}
