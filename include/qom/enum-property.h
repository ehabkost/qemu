/*
 * QOM enum property interfaces
 *
 * Author: Eduardo Habkost <ehabkost@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */
#ifndef QOM_ENUM_PROPERTY_H
#define QOM_ENUM_PROPERTY_H

#include "qom/field-property.h"

void field_prop_get_enum(Object *obj, Visitor *v, const char *name,
                         void *opaque, Error **errp);
void field_prop_set_enum(Object *obj, Visitor *v, const char *name,
                         void *opaque, Error **errp);

void field_prop_set_default_value_enum(ObjectProperty *op,
                                       const Property *prop,
                                       const QObject *defval);

#define PROP_ENUM(_state, _field, _defval, _prop, _type, ...) \
    PROP_SIGNED(_state, _field, _defval, _prop, _type,        \
                __VA_ARGS__)

#define DEFINE_PROP_ENUM(_n, ...) \
    PROP_ENUM(__VA_ARGS__, .name_template = (_n))

#endif
