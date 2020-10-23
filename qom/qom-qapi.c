#include "qemu/osdep.h"
#include "qom/qom-qapi.h"
#include "qapi/visitor.h"

DEFINE_QAPI_TYPE(qapi_int8, "int8", visit_type_int8);
DEFINE_QAPI_TYPE(qapi_int16, "int16", visit_type_int16);
DEFINE_QAPI_TYPE(qapi_int32, "int32", visit_type_int32);
DEFINE_QAPI_TYPE(qapi_int64, "int64", visit_type_int64);

DEFINE_QAPI_TYPE(qapi_uint8, "uint8", visit_type_uint8);
DEFINE_QAPI_TYPE(qapi_uint16, "uint16", visit_type_uint16);
DEFINE_QAPI_TYPE(qapi_uint32, "uint32", visit_type_uint32);
DEFINE_QAPI_TYPE(qapi_uint64, "uint64", visit_type_uint64);

DEFINE_QAPI_TYPE(qapi_bool, "bool", visit_type_bool);
DEFINE_QAPI_TYPE(qapi_str, "str", visit_type_str);

DEFINE_QAPI_TYPE(qapi_size, "size", visit_type_size);


bool qom_qapi_visit(const QAPITypeInfo *qapi_type,
                    Visitor *v, const char *name,
                    void *obj, size_t size, Error **errp)
{
    return qapi_type->visit(v, name, obj, size, errp);
}

const char *qom_qapi_type_name(const QAPITypeInfo *qapi_type)
{
    return qapi_type->type_name;
}
