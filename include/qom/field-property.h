/*
 * QOM field property API
 */
#ifndef QOM_FIELD_PROPERTY_H
#define QOM_FIELD_PROPERTY_H

#include "qom/object.h"
#include "qapi/util.h"

/**
 * Property:
 * @set_default: true if the default value should be set from @defval,
 *    in which case @info->set_default_value must not be NULL
 *    (if false then no default value is set by the property system
 *     and the field retains whatever value it was given by instance_init).
 * @defval: default value for the property. This is used only if @set_default
 *     is true.
 */
struct Property {
    /**
     * @name_template: Property name template
     *
     * This is a string containing the template to be used when
     * creating the property.  It can be NULL, and code shouldn't
     * assume it will contain the actual property name.
     */
    const char   *name_template;
    const PropertyInfo *info;
    ptrdiff_t    offset;
    uint8_t      bitnr;
    bool         set_default;
    union {
        int64_t i;
        uint64_t u;
    } defval;
    int          arrayoffset;
    const PropertyInfo *arrayinfo;
    int          arrayfieldsize;
    const char   *link_type;
};

struct PropertyInfo {
    const char *name;
    const char *description;
    const QEnumLookup *enum_table;
    int (*print)(Object *obj, Property *prop, char *dest, size_t len);
    void (*set_default_value)(ObjectProperty *op, const Property *prop);
    ObjectProperty *(*create)(ObjectClass *oc, const char *name,
                              Property *prop);
    ObjectPropertyAccessor *get;
    ObjectPropertyAccessor *set;
    ObjectPropertyRelease *release;
};

/**
 * object_class_property_add_field: Add a field property to object class
 * @oc: object class
 * @name: property name
 * @prop: property definition
 * @allow_set: check function called when property is set
 *
 * Add a field property to an object class.  A field property is
 * a property that will change a field at a specific offset of the
 * object instance struct.
 *
 * *@prop must exist for the life time of @oc.
 *
 * @allow_set should not be NULL.  If the property can always be
 * set, `prop_allow_set_always` can be used.  If the property can
 * never be set, `prop_allow_set_never` can be used.
 */
ObjectProperty *
object_class_property_add_field(ObjectClass *oc, const char *name,
                                Property *prop,
                                ObjectPropertyAllowSet allow_set);


/**
 * object_class_add_field_properties: Add field properties from array to a class
 * @oc: object class
 * @props: array of property definitions
 * @allow_set: check function called when property is set
 *
 * Register an array of field properties to a class, using
 * object_class_property_add_field() for each array element.
 *
 * The array at @props must end with DEFINE_PROP_END_OF_LIST(), and
 * must exist for the life time of @oc.
 *
 * @allow_set should not be NULL.  If the property can always be
 * set, `prop_allow_set_always` can be used.  If the property can
 * never be set, `prop_allow_set_never` can be used.
 */
void object_class_add_field_properties(ObjectClass *oc, Property *props,
                                       ObjectPropertyAllowSet allow_set);

void *object_field_prop_ptr(Object *obj, Property *prop);

#endif
