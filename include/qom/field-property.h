/*
 * QOM field property API
 */
#ifndef QOM_FIELD_PROPERTY_H
#define QOM_FIELD_PROPERTY_H

#include "qom/object.h"
#include "qapi/util.h"

/**
 * struct Property: definition of a field property
 *
 * Field properties are used to read and write fields in object
 * instance structs.  Field properties are defined using
 * ``DEFINE_PROP_<type>``.
 */
struct Property {
    /* private: */
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
    /**
     * @set_default: true if the default value should be set from @defval,
     *    in which case @info->set_default_value must not be NULL
     *    (if false then no default value is set by the property system
     *     and the field retains whatever value it was given by instance_init).
     */
    bool         set_default;
    /**
     * @defval: default value for the property. This is used only if @set_default
     *     is true.
     */
    union {
        int64_t i;
        uint64_t u;
    } defval;
    /* private: */
    int          arrayoffset;
    const PropertyInfo *arrayinfo;
    int          arrayfieldsize;
    const char   *link_type;
};

/**
 * struct PropertyInfo: information on a specific QOM property type
 */
struct PropertyInfo {
    /** @name: property type name */
    const char *name;
    /** @description: Description for help text */
    const char *description;
    /** @enum_table: Used by field_prop_get_enum() and field_prop_set_enum() */
    const QEnumLookup *enum_table;
    /** @print: String formatting function, for the human monitor */
    int (*print)(Object *obj, Property *prop, char *dest, size_t len);
    /** @set_default_value: Callback for initializing the default value */
    void (*set_default_value)(ObjectProperty *op, const Property *prop);
    /** @create: Optional callback for creation of property */
    ObjectProperty *(*create)(ObjectClass *oc, const char *name,
                              Property *prop);
    /**
     * @get: Property getter.  The opaque parameter will point to
     *        the &Property struct for the property.
     */
    ObjectPropertyAccessor *get;
    /**
     * @set: Property setter.  The opaque parameter will point to
     *        the &Property struct for the property.
     */
    ObjectPropertyAccessor *set;
    /**
     * @release: Optional release function, called when the object
     * is destroyed
     */
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
