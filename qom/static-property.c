/*
 * QOM static property API implementation
 */
#include "qemu/osdep.h"
#include "qom/static-property.h"
#include "qom/static-property-internal.h"

void *object_static_prop_ptr(Object *obj, Property *prop)
{
    void *ptr = obj;
    ptr += prop->offset;
    return ptr;
}

static bool static_prop_call_qapi_visitor(Object *obj, Property *prop,
                                          Visitor *v, const char *name,
                                          Error **errp)
{
    void *ptr = object_static_prop_ptr(obj, prop);

    return qom_qapi_visitor_call(prop->info->qapi_type,
                                 v, name, ptr, prop->size, errp);
}

static void static_prop_get(Object *obj, Visitor *v, const char *name,
                            void *opaque, Error **errp)
{
    Property *prop = opaque;
    if (prop->info->get) {
        return prop->info->get(obj, v, name, prop, errp);
    }
    if (prop->info->qapi_type) {
        static_prop_call_qapi_visitor(obj, prop, v, name, errp);
        return;
    }
    g_assert_not_reached();
}

/**
 * static_prop_getter: Return getter function to be used for property
 *
 * Return value can be NULL if @info has no getter function.
 */
static ObjectPropertyAccessor *static_prop_getter(const PropertyInfo *info)
{
    return (info->get || info->qapi_type) ? static_prop_get : NULL;
}

static void static_prop_set(Object *obj, Visitor *v, const char *name,
                            void *opaque, Error **errp)
{
    Property *prop = opaque;

    if (prop->info->set) {
        return prop->info->set(obj, v, name, prop, errp);
    }
    if (prop->info->qapi_type) {
        static_prop_call_qapi_visitor(obj, prop, v, name, errp);
        return;
    }
    g_assert_not_reached();
}

/**
 * static_prop_setter: Return setter function to be used for property
 *
 * Return value can be NULL if @info has not setter function.
 */
static ObjectPropertyAccessor *static_prop_setter(const PropertyInfo *info)
{
    return (info->set || info->qapi_type) ? static_prop_set : NULL;
}

static const char *static_prop_type_name(const PropertyInfo *info)
{
    assert(info->name || info->qapi_type);
    return info->name ? info->name : qom_qapi_type_name(info->qapi_type);
}

ObjectProperty *
object_property_add_static(Object *obj, Property *prop,
                           ObjectPropertyAllowSet allow_set)
{
    ObjectProperty *op;

    assert(!prop->info->create);

    op = object_property_add(obj, prop->name,
                             static_prop_type_name(prop->info),
                             static_prop_getter(prop->info),
                             static_prop_setter(prop->info),
                             prop->info->release,
                             prop);

    object_property_set_description(obj, prop->name,
                                    prop->info->description);

    if (prop->set_default) {
        prop->info->set_default_value(op, prop);
        if (op->init) {
            op->init(obj, op);
        }
    }

    op->allow_set = allow_set;
    return op;
}

ObjectProperty *
object_class_property_add_static(ObjectClass *oc, Property *prop,
                                 ObjectPropertyAllowSet allow_set)
{
    ObjectProperty *op;

    if (prop->info->create) {
        op = prop->info->create(oc, prop);
    } else {
        op = object_class_property_add(oc,
                                       prop->name,
                                       static_prop_type_name(prop->info),
                                       static_prop_getter(prop->info),
                                       static_prop_setter(prop->info),
                                       prop->info->release,
                                       prop);
    }
    if (prop->set_default) {
        prop->info->set_default_value(op, prop);
    }
    if (prop->info->description) {
        object_class_property_set_description(oc, prop->name,
                                            prop->info->description);
    }

    op->allow_set = allow_set;
    return op;
}

void object_class_add_static_props(ObjectClass *oc, Property *props,
                                   ObjectPropertyAllowSet allow_set)
{
    Property *prop;

    for (prop = props; prop && prop->name; prop++) {
        object_class_property_add_static(oc, prop, allow_set);
    }
}
