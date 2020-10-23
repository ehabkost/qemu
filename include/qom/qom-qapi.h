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
 * typedef QAPITypeInfo: Reference to a QAPI type
 *
 * `QAPITypeInfo` carries information about a QOM visitor, including:
 *
 * - A wrapper to a QAPI visitor, that can be called using
 *   qom_qapi_visitor_call()
 * - Storage size of the ``obj`` argument to the QAPI visitor
 * - A type name string that can be used when registering QOM properties
 *
 * In addition to the info carried by `QAPITypeInfo` at runtime, the
 * `DECLARE_QAPI_TYPE` macro will declare a typedef that can be used
 * through the `QOM_QAPI_C_TYPE` macro to get the storage type needed
 * for a QAPI type at build time.
 *
 * QOM visitor types are declared using `DECLARE_QAPI_TYPE`.
 */
typedef struct QAPITypeInfo QAPITypeInfo;

/**
 * QOM_QAPI_C_TYPE: expands to a typedef name for actual C type of QAPI type
 *
 * @name: QAPI type name declared using DECLARE_QAPI_TYPE
 */
#define QOM_QAPI_C_TYPE(name) QOM_QAPI_C_TYPE_##name

/**
 * DECLARE_QAPI_TYPE: Declare variable and typedef for QOM visitor type
 *
 * @name: Name of type
 * @c_type: C type of of the ``obj`` argument to the QAPI visitor
 *
 * Declares a variable named @name, of type #QOMVIsitorType, and a
 * typedef named ``QOM_QAPI_C_TYPE(name)``
 */
#define DECLARE_QAPI_TYPE(name, c_type) \
    extern const QAPITypeInfo name; \
    typedef c_type QOM_QAPI_C_TYPE(name);

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

/**
 * qom_qapi_visitor_call: Call visitor wrapper for a given type
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
bool qom_qapi_visitor_call(const QAPITypeInfo *qapi_type,
                           Visitor *v, const char *name,
                           void *obj, size_t size, Error **errp);

/** Return type name string for a QAPI type */
const char *qom_qapi_type_name(const QAPITypeInfo *qapi_type);

#endif
