/*
 * QEMU Object Model - QObject wrappers
 *
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * Author: Paolo Bonzini <pbonzini@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#ifndef QEMU_QOM_QOBJECT_H
#define QEMU_QOM_QOBJECT_H

#include "qapi/qmp/qobject.h"
#include "qapi/qmp/qnum.h"

/*
 * object_property_get_qobject:
 * @obj: the object
 * @name: the name of the property
 * @errp: returns an error if this function fails
 *
 * Returns: the value of the property, converted to QObject, or NULL if
 * an error occurs.
 */
struct QObject *object_property_get_qobject(Object *obj, const char *name,
                                            struct Error **errp);

/**
 * object_property_set_qobject:
 * @obj: the object
 * @name: the name of the property
 * @value: The value that will be written to the property.
 * @errp: returns an error if this function fails
 *
 * Writes a property to a object.
 *
 * Returns: %true on success, %false on failure.
 */
bool object_property_set_qobject(Object *obj,
                                 const char *name, struct QObject *value,
                                 struct Error **errp);

/**
 * object_property_add_const: Add read-only property returning a constant
 * @obj: the object to add a property to
 * @name: the name of the property
 * @value: QObject for the property value
 *
 * Ownership of @value is transferred to the property.
 */
void object_property_add_const(Object *obj, const char *name,
                               struct QObject *value);

/**
 * object_property_add_const_int: Add read-only property returning int constant
 * @obj: the object to add a property to
 * @name: the name of the property
 * @value: property value
 */
static inline void
object_property_add_const_int(Object *obj,
                              const char *name,
                              int64_t value)
{
    object_property_add_const(obj, name, QOBJECT(qnum_from_int(value)));
}

/**
 * object_property_add_const_uint: Add read-only property returning uint constant
 * @obj: the object to add a property to
 * @name: the name of the property
 * @value: property value
 */
static inline void
object_property_add_const_uint(Object *obj,
                               const char *name,
                               uint64_t value)
{
    object_property_add_const(obj, name, QOBJECT(qnum_from_uint(value)));
}

/**
 * object_class_property_add_const: Add read-only property returning a constant
 * @oc: the class to add a property to
 * @name: the name of the property
 * @value: QObject for the property value
 *
 * Ownership of @value is transferred to the property.
 */
void object_class_property_add_const(ObjectClass *oc, const char *name,
                                     struct QObject *value);

/**
 * object_class_property_add_const_int: Add read-only property returning int constant
 * @oc: the class to add a property to
 * @name: the name of the property
 * @value: property value
 */
static inline void
object_class_property_add_const_int(ObjectClass *oc,
                                    const char *name,
                                    int64_t value)
{
    object_class_property_add_const(oc, name, QOBJECT(qnum_from_int(value)));
}

/**
 * object_class_property_add_const_uint: Add read-only property returning uint constant
 * @oc: the class to add a property to
 * @name: the name of the property
 * @value: property value
 */
static inline void
object_class_property_add_const_uint(ObjectClass *oc,
                                     const char *name,
                                     uint64_t value)
{
    object_class_property_add_const(oc, name, QOBJECT(qnum_from_uint(value)));
}

#endif
