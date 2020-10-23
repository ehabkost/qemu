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
 * typedef QOMVisitorType: Reference to a QOM visitor
 *
 * `QOMVisitorType` carries information about a QOM visitor, including:
 *
 * - A wrapper to a QAPI visitor, that can be called using
 *   qom_visitor_type_name()
 * - Storage size of the ``obj`` argument to the QAPI visitor
 * - A type name string that can be used when registering QOM properties
 *
 * QOM visitor types are declared using `DECLARE_VISITOR_TYPE`.
 */
typedef struct QOMVisitorType QOMVisitorType;

/** name of typedef for actual C type of declared visitor type */
#define QOM_VISITOR_C_TYPE(name) QOM_VISITOR_C_TYPE_##name

/** name of QOMVisitorType variable for a given visitor type */
#define QOM_VISITOR(name) qom_visitor_##name

/**
 * DECLARE_VISITOR_TYPE: Declare variable and typedef for QOM visitor type
 *
 * @name: Name of type
 * @c_type: C type of of the ``obj`` argument to the QAPI visitor
 *
 * Declares a variable named ``QOM_VISITOR(name)`` of type
 * `QOMVIsitorType`, and a typedef named ``QOM_VISITOR_C_TYPE(name)``
 */
#define DECLARE_VISITOR_TYPE(name, c_type) \
    extern const QOMVisitorType qom_visitor_##name; \
    typedef c_type QOM_VISITOR_C_TYPE(name);

DECLARE_VISITOR_TYPE(uint8, uint8_t);
DECLARE_VISITOR_TYPE(uint16, uint16_t);
DECLARE_VISITOR_TYPE(uint32, uint32_t);
DECLARE_VISITOR_TYPE(uint64, uint64_t);

DECLARE_VISITOR_TYPE(int8, int8_t);
DECLARE_VISITOR_TYPE(int16, int16_t);
DECLARE_VISITOR_TYPE(int32, int32_t);
DECLARE_VISITOR_TYPE(int64, int64_t);

DECLARE_VISITOR_TYPE(bool, bool);
DECLARE_VISITOR_TYPE(str, char *);

DECLARE_VISITOR_TYPE(size, uint64_t);

/**
 * qom_visitor_call: Call visitor wrapper for a given type
 * @visitor_type: pointer to variable declared using `DECLARE_VISITOR_TYPE`
 * @v: visitor object
 * @name: name argument for visitor
 * @obj: pointer to area containing the value
 * @size: size of area containing value
 * @errp: pointer to error information
 *
 * @obj must point to a object of type ``c_type`` (the second
 * argument of `DECLARE_VISITOR_TYPE`).
 *
 * @size must be ``sizeof(c_type)``
 */
bool qom_visitor_call(const QOMVisitorType *visitor_type,
                      Visitor *v, const char *name,
                      void *obj, size_t size, Error **errp);

/** Return QOM type name for a visitor type */
const char *qom_visitor_type_name(const QOMVisitorType *visitor_type);

#endif
