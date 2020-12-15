/*
 * qdev property parsing
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#ifndef HW_CORE_QDEV_PROP_INTERNAL_H
#define HW_CORE_QDEV_PROP_INTERNAL_H

void field_prop_get_enum(Object *obj, Visitor *v, const char *name,
                         void *opaque, Error **errp);
void field_prop_set_enum(Object *obj, Visitor *v, const char *name,
                         void *opaque, Error **errp);

void field_prop_set_default_value_enum(ObjectProperty *op,
                                       const Property *prop);
void field_prop_set_default_value_int(ObjectProperty *op,
                                      const Property *prop);
void field_prop_set_default_value_uint(ObjectProperty *op,
                                       const Property *prop);

void field_prop_get_int32(Object *obj, Visitor *v, const char *name,
                          void *opaque, Error **errp);
void field_prop_get_size32(Object *obj, Visitor *v, const char *name,
                           void *opaque, Error **errp);

#endif
