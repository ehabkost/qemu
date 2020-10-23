/*
 * QEMU Object Model - QAPI glue
 *
 * Copyright (C) 2020 Red Hat, Inc.
 *
 * Author: Eduardo Habkost <ehabkost@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */
#ifndef QEMU_QOM_QAPI_H
#define QEMU_QOM_QAPI_H

/**
 * typedef QAPITypeInfo: Information about a QAPI type
 *
 * `QAPITypeInfo` carries information about a QAPI type, including:
 *
 * - A reference to the QAPI visitor function for the type, that
 *   can be called using qom_qapi_visitor_call()
 * - Storage size of the ``obj`` argument to the QAPI visitor function
 * - A type name string that can be used when registering QOM properties
 *
 * In addition to the info carried by `QAPITypeInfo` at runtime, the
 * `DECLARE_QAPI_TYPE` macro will declare a typedef that can be used
 * through the `QOM_QAPI_C_TYPE` macro to get the storage type needed
 * for a QAPI type at compilation time.  This can be combined with the
 * type_check() macro to make sure variables or struct field types
 * match the one expected by the QAPI type.
 *
 * QOM visitor types are declared using `DECLARE_QAPI_TYPE`,
 * and defined using `DEFINE_QAPI_TYPE`.
 */
typedef struct QAPITypeInfo QAPITypeInfo;

/**
 * QOM_QAPI_C_TYPE: expands to a typedef name for actual C type of QAPI type
 *
 * @name: QAPI type name declared using `DECLARE_QAPI_TYPE`
 */
#define QOM_QAPI_C_TYPE(name) QOM_QAPI_C_TYPE_##name

/**
 * DECLARE_QAPI_TYPE: Declare variable and typedef for QOM visitor type
 *
 * @var_name: name for variable
 * @c_type: C type of of the ``obj`` argument to the QAPI visitor
 *
 * Declares a variable named @var_name, of type #QAPITypeInfo, and a
 * typedef named ``QOM_QAPI_C_TYPE(var_name)``
 */
#define DECLARE_QAPI_TYPE(var_name, c_type) \
    extern const QAPITypeInfo var_name;     \
    typedef c_type QOM_QAPI_C_TYPE(var_name);

typedef bool QOMVisitorWrapper(Visitor *v, const char *name,
                               void *obj, size_t size, Error **errp);

struct QAPITypeInfo
{
    /* data type name */
    const char *type_name;
    /* Expected size of memory area holding type */
    size_t size;

    /* Visitor wrapper function */
    QOMVisitorWrapper *visit;
};

/**
 * DEFINE_QAPI_TYPE: Define #QAPITypeInfo variable
 *
 * @var_name: name of variable declared using `DECLARE_QAPI_TYPE`
 * @name_str: user-visible name of QOM type
 * @qapi_visitor: visitor function
 *
 * In most cases, @qapi_visitor would be a QAPI-provided visitor.
 * It should have the following signature::
 *
 *   bool visit_type_NAME(Visitor *v, const char *name, T *obj,
 *                        Error **errp);
 *
 * Where ``T`` is the ``c_type`` argument for `DECLARE_QAPI_TYPE`.
 */
#define DEFINE_QAPI_TYPE(var_name, name_str, qapi_visitor)                       \
    static bool visitor_wrapper_##var_name(Visitor *v, const char *name,         \
                                           void *obj, size_t size, Error **errp) \
    {                                                                            \
        QOM_QAPI_C_TYPE(var_name) *ptr = obj;                                    \
        assert(size == sizeof(QOM_QAPI_C_TYPE(var_name)));                       \
        return qapi_visitor(v, name, ptr, errp);                                 \
    }                                                                            \
    const QAPITypeInfo var_name = {                                              \
        .type_name = name_str,                                                   \
        .size = sizeof(QOM_QAPI_C_TYPE(var_name)),                               \
        .visit = visitor_wrapper_##var_name,                                     \
    };

/**
 * qom_qapi_visit: Call visitor function for a given type
 * @qapi_type: pointer to variable declared using `DECLARE_QAPI_TYPE`
 * @v: visitor object
 * @name: name argument for visitor
 * @obj: pointer to area containing the value
 * @size: size of area containing value
 * @errp: pointer to error information
 *
 * @obj must point to a object of type ``c_type`` (the second
 * argument of `DECLARE_QAPI_TYPE`).
 *
 * @size must be ``sizeof(c_type)``
 */
bool qom_qapi_visit(const QAPITypeInfo *qapi_type,
                    Visitor *v, const char *name,
                    void *obj, size_t size, Error **errp);

/**
 * qom_qapi_type_name: Get type name string for a QAPI type
 * @qapi_type: pointer to #QAPITypeInfo variable
 *
 * Return the type name string passed as the `name_str` argument
 * to `DEFINE_QAPI_TYPE`.
 */
const char *qom_qapi_type_name(const QAPITypeInfo *qapi_type);

DECLARE_QAPI_TYPE(qapi_uint8, uint8_t);
DECLARE_QAPI_TYPE(qapi_uint16, uint16_t);
DECLARE_QAPI_TYPE(qapi_uint32, uint32_t);
DECLARE_QAPI_TYPE(qapi_uint64, uint64_t);

DECLARE_QAPI_TYPE(qapi_int8, int8_t);
DECLARE_QAPI_TYPE(qapi_int16, int16_t);
DECLARE_QAPI_TYPE(qapi_int32, int32_t);
DECLARE_QAPI_TYPE(qapi_int64, int64_t);

DECLARE_QAPI_TYPE(qapi_bool, bool);
DECLARE_QAPI_TYPE(qapi_str, char *);

DECLARE_QAPI_TYPE(qapi_size, uint64_t);

#endif
