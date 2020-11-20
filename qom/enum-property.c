/*
 * QOM enum property implementation
 *
 * Author: Eduardo Habkost <ehabkost@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qom/field-property.h"
#include "qom/enum-property.h"
#include "qapi/qapi-types-common.h"
#include "qapi/visitor.h"

void field_prop_get_enum(Object *obj, Visitor *v, const char *name,
                         void *opaque, Error **errp)
{
    Property *prop = opaque;
    int *ptr = object_field_prop_ptr(obj, prop);

    visit_type_enum(v, name, ptr, prop->info->enum_table, errp);
}

void field_prop_set_enum(Object *obj, Visitor *v, const char *name,
                         void *opaque, Error **errp)
{
    Property *prop = opaque;
    int *ptr = object_field_prop_ptr(obj, prop);

    visit_type_enum(v, name, ptr, prop->info->enum_table, errp);
}

void field_prop_set_default_value_enum(ObjectProperty *op,
                                       const Property *prop,
                                       const QObject *defval)
{
    QNum *qn = qobject_to(QNum, defval);
    object_property_set_default_str(op,
        qapi_enum_lookup(prop->info->enum_table, qnum_get_int(qn)));
}

const PropertyInfo prop_info_enum = {
    .name  = "enum",
    .get   = field_prop_get_enum,
    .set   = field_prop_set_enum,
    .set_default_value = field_prop_set_default_value_enum,
};

/* --- on/off/auto --- */

const PropertyInfo prop_info_on_off_auto = {
    .name = "OnOffAuto",
    .description = "on/off/auto",
    .enum_table = &OnOffAuto_lookup,
    .get = field_prop_get_enum,
    .set = field_prop_set_enum,
    .set_default_value = field_prop_set_default_value_enum,
};
