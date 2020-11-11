/*
 * QEMU Object Model - QObject wrappers
 *
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * Author: Paolo Bonzini <pbonzini@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qom/object.h"
#include "qapi/qmp/qobject.h"
#include "qapi/qmp/qnum.h"
#include "qom/qom-qobject.h"
#include "qapi/visitor.h"
#include "qapi/qobject-input-visitor.h"
#include "qapi/qobject-output-visitor.h"

bool object_property_set_qobject(Object *obj,
                                 const char *name, QObject *value,
                                 Error **errp)
{
    Visitor *v;
    bool ok;

    v = qobject_input_visitor_new(value);
    ok = object_property_set(obj, name, v, errp);
    visit_free(v);
    return ok;
}

QObject *object_property_get_qobject(Object *obj, const char *name,
                                     Error **errp)
{
    QObject *ret = NULL;
    Visitor *v;

    v = qobject_output_visitor_new(&ret);
    if (object_property_get(obj, name, v, errp)) {
        visit_complete(v, &ret);
    }
    visit_free(v);
    return ret;
}

static void const_property_get(Object *obj,
                               Visitor *v,
                               const char *name,
                               void *opaque,
                               Error **errp)
{
    QObject *value = opaque;
    visit_type_any(v, name, &value, errp);
}

static void const_property_release(Object *obj,
                                   const char *name,
                                   void *opaque)
{
    QObject *value = opaque;
    qobject_unref(value);
}

static const char *qnum_type_name(const QNum *value)
{
    switch (qnum_kind(value)) {
    case QNUM_I64:
        return "int64";
    case QNUM_U64:
        return "uint64";
    default:
        g_assert_not_reached();
    }
}

static const char *qobject_type_name(const QObject *value)
{
    switch (qobject_type(value)) {
    case QTYPE_QNUM:
        return qnum_type_name(qobject_to(QNum, value));
    case QTYPE_QBOOL:
        return "bool";
    case QTYPE_QSTRING:
        return "string";
    case QTYPE_QDICT:
        return "dict";
    case QTYPE_QLIST:
        return "list";
    default:
        g_assert_not_reached();
    }
}

void object_property_add_const(Object *obj, const char *name,
                               struct QObject *value)
{
    object_property_add(obj, name, qobject_type_name(value),
                        const_property_get,
                        NULL,
                        const_property_release,
                        value);
}

void object_class_property_add_const(ObjectClass *oc, const char *name,
                                     struct QObject *value)
{
    object_class_property_add(oc, name, qobject_type_name(value),
                              const_property_get,
                              NULL,
                              NULL,
                              value);
}
