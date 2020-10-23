/*
 * QOM field property API implementation
 */
#include "qemu/osdep.h"
#include "qom/field-property.h"
#include "qom/field-property-internal.h"

void *object_field_prop_ptr(Object *obj, Property *prop, size_t expected_size)
{
    void *ptr = obj;
    ptr += prop->offset;
    assert(prop->size == expected_size);
    return ptr;
}

static bool field_prop_call_qapi_visitor(Object *obj, Property *prop,
                                          Visitor *v, const char *name,
                                          Error **errp)
{
    void *ptr = object_field_prop_ptr(obj, prop,
                                      prop->qapi_type->size);

    return qom_qapi_visit(prop->qapi_type,
                          v, name, ptr, prop->size, errp);
}

static void field_prop_get(Object *obj, Visitor *v, const char *name,
                           void *opaque, Error **errp)
{
    Property *prop = opaque;
    if (prop->info->get) {
        return prop->info->get(obj, v, name, opaque, errp);
    }
    if (prop->qapi_type) {
        field_prop_call_qapi_visitor(obj, prop, v, name, errp);
        return;
    }
    g_assert_not_reached();
}

/**
 * field_prop_getter: Return getter function to be used for property
 *
 * Return value can be NULL if the property won't be readable.
 */
static ObjectPropertyAccessor *field_prop_getter(const Property *prop)
{
    if (prop->info->get || prop->qapi_type) {
        return field_prop_get;
    } else {
        return NULL;
    }
}

static void field_prop_set(Object *obj, Visitor *v, const char *name,
                           void *opaque, Error **errp)
{
    Property *prop = opaque;

    if (prop->info->set) {
        return prop->info->set(obj, v, name, opaque, errp);
    }
    if (prop->qapi_type) {
        field_prop_call_qapi_visitor(obj, prop, v, name, errp);
        return;
    }
    g_assert_not_reached();
}

/**
 * field_prop_setter: Return setter function to be used for property
 *
 * Return value can be NULL if the property won't be writable.
 */
static ObjectPropertyAccessor *field_prop_setter(const Property *prop)
{
    if (prop->info->set || prop->qapi_type) {
        return field_prop_set;
    } else {
        return NULL;
    }
}

/**
 * field_prop_type_name: Return type name to be used for property
 */
static const char *field_prop_type_name(const Property *prop)
{
    if (prop->info->name) {
        return prop->info->name;
    }
    if (prop->qapi_type) {
        return qom_qapi_type_name(prop->qapi_type);
    }
    g_assert_not_reached();
}

static void field_prop_release(Object *obj, const char *name, void *opaque)
{
    Property *prop = opaque;
    if (prop->info->release) {
        prop->info->release(obj, name, prop);
    }
}

static void field_prop_set_default_value(ObjectProperty *op,
                                         Property *prop)
{
    if (qlit_type(&prop->defval) == QTYPE_QNULL) {
        return;
    }

    if (prop->info->set_default_value) {
        prop->info->set_default_value(op, prop);
    } else {
        object_property_set_default(op, qobject_from_qlit(&prop->defval));
    }
}

/* Finish initialization of field property */
static void field_prop_finish_init(ObjectProperty *op, Property *prop,
                                   ObjectPropertyAllowSet allow_set)
{
    if (prop->description) {
        property_set_description(op, prop->description);
    } else if (prop->info->description) {
        property_set_description(op, prop->info->description);
    }

    field_prop_set_default_value(op, prop);

    op->allow_set = allow_set;
}

ObjectProperty *
object_property_add_field(Object *obj, const char *name,
                          Property *prop,
                          ObjectPropertyAllowSet allow_set)
{
    ObjectProperty *op;

    assert(allow_set);
    assert(!prop->info || !prop->info->create);

    op = object_property_add(obj, name,
                             field_prop_type_name(prop),
                             field_prop_getter(prop),
                             field_prop_setter(prop),
                             field_prop_release,
                             prop);

    field_prop_finish_init(op, prop, allow_set);
    if (op->init) {
        op->init(obj, op);
    }
    return op;
}

ObjectProperty *
object_class_property_add_field(ObjectClass *oc, const char *name,
                                Property *prop,
                                ObjectPropertyAllowSet allow_set)
{
    ObjectProperty *op;

    assert(allow_set);

    if (prop->info->create) {
        op = prop->info->create(oc, name, prop);
    } else {
        op = object_class_property_add(oc, name,
                                       field_prop_type_name(prop),
                                       field_prop_getter(prop),
                                       field_prop_setter(prop),
                                       field_prop_release,
                                       prop);
    }

    field_prop_finish_init(op, prop, allow_set);
    return op;
}

void object_class_add_field_properties(ObjectClass *oc, Property *props,
                                        ObjectPropertyAllowSet allow_set)
{
    Property *prop;

    for (prop = props; prop && prop->name_template; prop++) {
        object_class_property_add_field(oc, prop->name_template, prop,
                                        allow_set);
    }
}
