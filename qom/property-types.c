#include "qemu/osdep.h"
#include "qom/static-property.h"
#include "qom/static-property-internal.h"
#include "qapi/qapi-types-common.h"
#include "qapi/visitor.h"
#include "qapi/error.h"
#include "qemu/uuid.h"

void object_propinfo_get_enum(Object *obj, Visitor *v, const char *name,
                            void *opaque, Error **errp)
{
    Property *prop = opaque;
    int *pvalue = object_static_prop_ptr(obj, prop);

    visit_type_enum(v, name, pvalue, prop->info->enum_table, errp);
}

void object_propinfo_set_enum(Object *obj, Visitor *v, const char *name,
                            void *opaque, Error **errp)
{
    Property *prop = opaque;
    int *pvalue = object_static_prop_ptr(obj, prop);

    visit_type_enum(v, name, pvalue, prop->info->enum_table, errp);
}

void object_propinfo_set_default_value_enum(ObjectProperty *op,
                                          const Property *prop)
{
    object_property_set_default_str(op,
        qapi_enum_lookup(prop->info->enum_table, prop->defval.i));
}

const PropertyInfo prop_info_enum = {
    .name  = "enum",
    .get   = object_propinfo_get_enum,
    .set   = object_propinfo_set_enum,
    .set_default_value = object_propinfo_set_default_value_enum,
};

/* Bit */

static uint32_t qdev_get_prop_mask(Property *prop)
{
    assert(prop->info == &prop_info_bit);
    return 0x1 << prop->bitnr;
}

static void bit_prop_set(Object *obj, Property *props, bool val)
{
    uint32_t *p = object_static_prop_ptr(obj, props);
    uint32_t mask = qdev_get_prop_mask(props);
    if (val) {
        *p |= mask;
    } else {
        *p &= ~mask;
    }
}

static void prop_get_bit(Object *obj, Visitor *v, const char *name,
                         void *opaque, Error **errp)
{
    Property *prop = opaque;
    uint32_t *p = object_static_prop_ptr(obj, prop);
    bool value = (*p & qdev_get_prop_mask(prop)) != 0;

    visit_type_bool(v, name, &value, errp);
}

static void prop_set_bit(Object *obj, Visitor *v, const char *name,
                         void *opaque, Error **errp)
{
    Property *prop = opaque;
    bool value;

    if (!visit_type_bool(v, name, &value, errp)) {
        return;
    }
    bit_prop_set(obj, prop, value);
}

static void set_default_value_bool(ObjectProperty *op, const Property *prop)
{
    object_property_set_default_bool(op, prop->defval.u);
}

const PropertyInfo prop_info_bit = {
    .name  = "bool",
    .description = "on/off",
    .get   = prop_get_bit,
    .set   = prop_set_bit,
    .set_default_value = set_default_value_bool,
};

/* Bit64 */

static uint64_t qdev_get_prop_mask64(Property *prop)
{
    assert(prop->info == &prop_info_bit64);
    return 0x1ull << prop->bitnr;
}

static void bit64_prop_set(Object *obj, Property *props, bool val)
{
    uint64_t *p = object_static_prop_ptr(obj, props);
    uint64_t mask = qdev_get_prop_mask64(props);
    if (val) {
        *p |= mask;
    } else {
        *p &= ~mask;
    }
}

static void prop_get_bit64(Object *obj, Visitor *v, const char *name,
                           void *opaque, Error **errp)
{
    Property *prop = opaque;
    uint64_t *p = object_static_prop_ptr(obj, prop);
    bool value = (*p & qdev_get_prop_mask64(prop)) != 0;

    visit_type_bool(v, name, &value, errp);
}

static void prop_set_bit64(Object *obj, Visitor *v, const char *name,
                           void *opaque, Error **errp)
{
    Property *prop = opaque;
    bool value;

    if (!visit_type_bool(v, name, &value, errp)) {
        return;
    }
    bit64_prop_set(obj, prop, value);
}

const PropertyInfo prop_info_bit64 = {
    .name  = "bool",
    .description = "on/off",
    .get   = prop_get_bit64,
    .set   = prop_set_bit64,
    .set_default_value = set_default_value_bool,
};

/* --- bool --- */

const PropertyInfo prop_info_bool = {
    .qapi_type = &qapi_bool,
    .set_default_value = set_default_value_bool,
};

/* --- 8bit integer --- */

void object_propinfo_set_default_value_int(ObjectProperty *op,
                                         const Property *prop)
{
    object_property_set_default_int(op, prop->defval.i);
}

void object_propinfo_set_default_value_uint(ObjectProperty *op,
                                          const Property *prop)
{
    object_property_set_default_uint(op, prop->defval.u);
}

const PropertyInfo prop_info_uint8 = {
    .qapi_type = &qapi_uint8,
    .set_default_value = object_propinfo_set_default_value_uint,
};

/* --- 16bit integer --- */

const PropertyInfo prop_info_uint16 = {
    .qapi_type = &qapi_uint16,
    .set_default_value = object_propinfo_set_default_value_uint,
};

/* --- 32bit integer --- */

const PropertyInfo prop_info_uint32 = {
    .qapi_type = &qapi_uint32,
    .set_default_value = object_propinfo_set_default_value_uint,
};

const PropertyInfo prop_info_int32 = {
    .qapi_type = &qapi_int32,
    .set_default_value = object_propinfo_set_default_value_int,
};

/* --- 64bit integer --- */

const PropertyInfo prop_info_uint64 = {
    .qapi_type = &qapi_uint64,
    .set_default_value = object_propinfo_set_default_value_uint,
};

const PropertyInfo prop_info_int64 = {
    .qapi_type = &qapi_int64,
    .set_default_value = object_propinfo_set_default_value_int,
};

const PropertyInfo qdev_prop_array_pointer = {
    .name = "array",
    /*
     * We don't register an actual property for array pointers, but
     * we use the "array" property type to track the array pointer
     * field.
     */
};

/* --- string --- */

static void release_string(Object *obj, const char *name, void *opaque)
{
    Property *prop = opaque;
    g_free(*(char **)object_static_prop_ptr(obj, prop));
}

static void get_string(Object *obj, Visitor *v, const char *name,
                       void *opaque, Error **errp)
{
    Property *prop = opaque;
    char **pstr = object_static_prop_ptr(obj, prop);

    if (!*pstr) {
        char *str = (char *)"";
        visit_type_str(v, name, &str, errp);
    } else {
        visit_type_str(v, name, pstr, errp);
    }
}

static void set_string(Object *obj, Visitor *v, const char *name,
                       void *opaque, Error **errp)
{
    Property *prop = opaque;
    char **pstr = object_static_prop_ptr(obj, prop);
    char *str;

    if (!visit_type_str(v, name, &str, errp)) {
        return;
    }
    g_free(*pstr);
    *pstr = str;
}

const PropertyInfo prop_info_string = {
    .name  = "str",
    .release = release_string,
    .get   = get_string,
    .set   = set_string,
};

/* --- on/off/auto --- */

const PropertyInfo prop_info_on_off_auto = {
    .name = "OnOffAuto",
    .description = "on/off/auto",
    .enum_table = &OnOffAuto_lookup,
    .get = object_propinfo_get_enum,
    .set = object_propinfo_set_enum,
    .set_default_value = object_propinfo_set_default_value_enum,
};

/* --- 32bit unsigned int 'size' type --- */

void object_propinfo_get_size32(Object *obj, Visitor *v, const char *name,
                              void *opaque, Error **errp)
{
    Property *prop = opaque;
    uint32_t *pvalue = object_static_prop_ptr(obj, prop);
    uint64_t value = *pvalue;

    visit_type_size(v, name, &value, errp);
}

static void set_size32(Object *obj, Visitor *v, const char *name, void *opaque,
                       Error **errp)
{
    Property *prop = opaque;
    uint32_t *pvalue = object_static_prop_ptr(obj, prop);
    uint64_t value;

    if (!visit_type_size(v, name, &value, errp)) {
        return;
    }

    if (value > UINT32_MAX) {
        error_setg(errp,
                   "Property %s.%s doesn't take value %" PRIu64
                   " (maximum: %u)",
                   object_get_typename(obj), name, value, UINT32_MAX);
        return;
    }

    *pvalue = value;
}

const PropertyInfo prop_info_size32 = {
    .name  = "size",
    .get = object_propinfo_get_size32,
    .set = set_size32,
    .set_default_value = object_propinfo_set_default_value_uint,
};

/* --- UUID --- */

static void get_uuid(Object *obj, Visitor *v, const char *name, void *opaque,
                     Error **errp)
{
    Property *prop = opaque;
    QemuUUID *uuid = object_static_prop_ptr(obj, prop);
    char buffer[UUID_FMT_LEN + 1];
    char *p = buffer;

    qemu_uuid_unparse(uuid, buffer);

    visit_type_str(v, name, &p, errp);
}

#define UUID_VALUE_AUTO        "auto"

static void set_uuid(Object *obj, Visitor *v, const char *name, void *opaque,
                    Error **errp)
{
    Property *prop = opaque;
    QemuUUID *uuid = object_static_prop_ptr(obj, prop);
    char *str;

    if (!visit_type_str(v, name, &str, errp)) {
        return;
    }

    if (!strcmp(str, UUID_VALUE_AUTO)) {
        qemu_uuid_generate(uuid);
    } else if (qemu_uuid_parse(str, uuid) < 0) {
        error_setg(errp, "invalid UUID: '%s'", str);
    }
    g_free(str);
}

static void set_default_uuid_auto(ObjectProperty *op, const Property *prop)
{
    object_property_set_default_str(op, UUID_VALUE_AUTO);
}

const PropertyInfo prop_info_uuid = {
    .name  = "str",
    .description = "UUID (aka GUID) or \"" UUID_VALUE_AUTO
        "\" for random value (default)",
    .get   = get_uuid,
    .set   = set_uuid,
    .set_default_value = set_default_uuid_auto,
};

/* --- support for array properties --- */

/* Used as an opaque for the object properties we add for each
 * array element. Note that the struct Property must be first
 * in the struct so that a pointer to this works as the opaque
 * for the underlying element's property hooks as well as for
 * our own release callback.
 */
typedef struct {
    struct Property prop;
    char *propname;
    ObjectPropertyRelease *release;
} ArrayElementProperty;

/**
 * Create ArrayElementProperty based on array length property
 * @array_len_prop (which was previously defined using DEFINE_PROP_ARRAY()).
 */
static ArrayElementProperty *array_element_new(Object *obj,
                                               Property *array_len_prop,
                                               const char *arrayname,
                                               int index,
                                               void *eltptr)
{
    char *propname = g_strdup_printf("%s[%d]", arrayname, index);
    ArrayElementProperty *arrayprop = g_new0(ArrayElementProperty, 1);
    arrayprop->propname = propname;
    arrayprop->prop.info = array_len_prop->arrayinfo;
    arrayprop->prop.name = propname;
    /* This ugly piece of pointer arithmetic sets up the offset so
     * that when the underlying get/set hooks call qdev_get_prop_ptr
     * they get the right answer despite the array element not actually
     * being inside the device struct.
     */
    arrayprop->prop.offset = eltptr - (void *)obj;
    arrayprop->prop.size = array_len_prop->arrayfieldsize;
    assert(object_static_prop_ptr(obj, &arrayprop->prop) == eltptr);
    return arrayprop;
}

/* object property release callback for array element properties:
 * we call the underlying element's property release hook, and
 * then free the memory we allocated when we added the property.
 */
static void array_element_release(Object *obj, const char *name, void *opaque)
{
    ArrayElementProperty *p = opaque;
    if (p->release) {
        p->release(obj, name, opaque);
    }
    g_free(p->propname);
    g_free(p);
}

static void object_property_add_array_element(Object *obj,
                                              Property *array_len_prop,
                                              ArrayElementProperty *prop)
{
    ObjectProperty *array_op = object_property_find(obj, array_len_prop->name);
    ObjectProperty *op = object_property_add_static(obj, &prop->prop,
                                                    array_op->allow_set);

    assert((void *)prop == (void *)&prop->prop);
    prop->release = op->release;
    /* array_element_release() will call the original release function */
    op->release = array_element_release;
}

static void set_prop_arraylen(Object *obj, Visitor *v, const char *name,
                              void *opaque, Error **errp)
{
    /* Setter for the property which defines the length of a
     * variable-sized property array. As well as actually setting the
     * array-length field in the device struct, we have to create the
     * array itself and dynamically add the corresponding properties.
     */
    Property *prop = opaque;
    uint32_t *alenptr = object_static_prop_ptr(obj, prop);
    void **arrayptr = object_static_prop_ptr(obj, prop->array_pointer);
    void *eltptr;
    const char *arrayname;
    int i;

    if (*alenptr) {
        error_setg(errp, "array size property %s may not be set more than once",
                   name);
        return;
    }
    if (!visit_type_uint32(v, name, alenptr, errp)) {
        return;
    }
    if (!*alenptr) {
        return;
    }

    /* DEFINE_PROP_ARRAY guarantees that name should start with this prefix;
     * strip it off so we can get the name of the array itself.
     */
    assert(strncmp(name, PROP_ARRAY_LEN_PREFIX,
                   strlen(PROP_ARRAY_LEN_PREFIX)) == 0);
    arrayname = name + strlen(PROP_ARRAY_LEN_PREFIX);

    /* Note that it is the responsibility of the individual device's deinit
     * to free the array proper.
     */
    *arrayptr = eltptr = g_malloc0(*alenptr * prop->arrayfieldsize);
    for (i = 0; i < *alenptr; i++, eltptr += prop->arrayfieldsize) {
        ArrayElementProperty *elt_prop = array_element_new(obj, prop, arrayname,
                                                           i, eltptr);
        object_property_add_array_element(obj, prop, elt_prop);
    }
}

const PropertyInfo prop_info_arraylen = {
    .qapi_type = &qapi_uint32,
    .set = set_prop_arraylen,
    .set_default_value = object_propinfo_set_default_value_uint,
};

/* --- 64bit unsigned int 'size' type --- */

const PropertyInfo prop_info_size = {
    .qapi_type = &qapi_size,
    .set_default_value = object_propinfo_set_default_value_uint,
};

/* --- object link property --- */

static ObjectProperty *create_link_property(ObjectClass *oc, Property *prop)
{
    /*
     * NOTE: object_property_allow_set_link is unconditional, but
     *       ObjectProperty.allow_set may be set for the property too.
     */
    return object_class_property_add_link(oc, prop->name, prop->link_type,
                                          prop->offset,
                                          object_property_allow_set_link,
                                          OBJ_PROP_LINK_STRONG);
}

const PropertyInfo prop_info_link = {
    .name = "link",
    .create = create_link_property,
};
