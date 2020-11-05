/*
 * QOM field property API
 */
#ifndef QOM_FIELD_PROPERTY_H
#define QOM_FIELD_PROPERTY_H

#include "qom/object.h"
#include "qapi/util.h"
#include "qapi/qmp/qlit.h"

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
    /** @offset: offset of field in object instance struct */
    ptrdiff_t    offset;
    /** @size: size of field in object instance struct */
    size_t       size;
    uint8_t      bitnr;
    /** @defval: If not QNull, the default value for the property */
    QLitObject defval;
    /** @description: Property description, for help text */
    const char *description;
    /* private: */
    int          arrayoffset;
    const PropertyInfo *arrayinfo;
    int          arrayfieldsize;
    const char   *link_type;
};

/**
 * typedef FieldAccessor: a field property getter or setter function
 * @obj: the object instance
 * @v: the visitor that contains the property data
 * @name: the name of the property
 * @prop: Field property definition
 * @errp: pointer to error information
 */
typedef void FieldAccessor(Object *obj, Visitor *v,
                           const char *name, Property *prop,
                           Error **errp);

/**
 * typedef FieldRelease:
 * @obj: the object instance
 * @name: the name of the property
 * @prop: Field property definition
 */
typedef void FieldRelease(Object *obj, const char *name, Property *prop);


/**
 * struct PropertyInfo: information on a specific QOM property type
 */
struct PropertyInfo {
    /** @name: property type name */
    const char *name;
    /**
     * @description: Default property description
     *
     * Property.description may override it for individual properties.
     */
    const char *description;
    /** @enum_table: Used by field_prop_get_enum() and field_prop_set_enum() */
    const QEnumLookup *enum_table;
    /** @print: String formatting function, for the human monitor */
    int (*print)(Object *obj, Property *prop, char *dest, size_t len);
    /**
     * @set_default_value: Optional callback for initializing the default value
     *
     * Most property types don't need to set this, as by default
     * object_property_set_default() is called with the value at
     * Property.defval.
     */
    void (*set_default_value)(ObjectProperty *op, const Property *prop);
    /** @create: Optional callback for creation of property */
    ObjectProperty *(*create)(ObjectClass *oc, const char *name,
                              Property *prop);
    /** @get: Property getter */
    FieldAccessor *get;
    /** @set: Property setter */
    FieldAccessor *set;
    /**
     * @release: Optional release function, called when the object
     * is destroyed
     */
    FieldRelease *release;
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

/**
 * object_field_prop_ptr: Get pointer to property field
 * @obj: the object instance
 * @prop: field property definition
 * @expected_size: expected size of struct field
 *
 * Don't use this function directly, use the FIELD_PTR() macro instead.
 */
void *object_field_prop_ptr(Object *obj, Property *prop, size_t expected_size);

/**
 * FIELD_PTR: Get pointer to struct field for property
 * @obj: the object instance (pointer to #Object)
 * @prop: field property definition (pointer to #Property)
 * @type: type of the struct field associated with the property
 *        (a C type name or type expression)
 *
 * This returns a pointer to type @type, pointing to the struct
 * field containing the property value.
 *
 * @type must match the expected type for the property.
 */
#define FIELD_PTR(obj, prop, type) \
    ((type *)object_field_prop_ptr((obj), (prop), sizeof(type)))

#endif
