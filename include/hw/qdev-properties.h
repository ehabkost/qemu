#ifndef QEMU_QDEV_PROPERTIES_H
#define QEMU_QDEV_PROPERTIES_H

#include "hw/qdev-core.h"

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
    const char   *name;
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
    ObjectProperty *(*create)(ObjectClass *oc, Property *prop);
    ObjectPropertyAccessor *get;
    ObjectPropertyAccessor *set;
    ObjectPropertyRelease *release;
};


/*** qdev-properties.c ***/

extern const PropertyInfo qdev_prop_bit;
extern const PropertyInfo qdev_prop_bit64;
extern const PropertyInfo qdev_prop_bool;
extern const PropertyInfo qdev_prop_enum;
extern const PropertyInfo qdev_prop_uint8;
extern const PropertyInfo qdev_prop_uint16;
extern const PropertyInfo qdev_prop_uint32;
extern const PropertyInfo qdev_prop_int32;
extern const PropertyInfo qdev_prop_uint64;
extern const PropertyInfo qdev_prop_int64;
extern const PropertyInfo qdev_prop_size;
extern const PropertyInfo qdev_prop_string;
extern const PropertyInfo qdev_prop_chr;
extern const PropertyInfo qdev_prop_tpm;
extern const PropertyInfo qdev_prop_macaddr;
extern const PropertyInfo qdev_prop_reserved_region;
extern const PropertyInfo qdev_prop_on_off_auto;
extern const PropertyInfo qdev_prop_multifd_compression;
extern const PropertyInfo qdev_prop_losttickpolicy;
extern const PropertyInfo qdev_prop_blockdev_on_error;
extern const PropertyInfo qdev_prop_bios_chs_trans;
extern const PropertyInfo qdev_prop_fdc_drive_type;
extern const PropertyInfo qdev_prop_drive;
extern const PropertyInfo qdev_prop_drive_iothread;
extern const PropertyInfo qdev_prop_netdev;
extern const PropertyInfo qdev_prop_pci_devfn;
extern const PropertyInfo qdev_prop_size32;
extern const PropertyInfo qdev_prop_blocksize;
extern const PropertyInfo qdev_prop_pci_host_devaddr;
extern const PropertyInfo qdev_prop_uuid;
extern const PropertyInfo qdev_prop_arraylen;
extern const PropertyInfo qdev_prop_audiodev;
extern const PropertyInfo qdev_prop_link;
extern const PropertyInfo qdev_prop_off_auto_pcibar;
extern const PropertyInfo qdev_prop_pcie_link_speed;
extern const PropertyInfo qdev_prop_pcie_link_width;

#define DEFINE_PROP(_name, _state, _field, _prop, _type) { \
        .name      = (_name),                                    \
        .info      = &(_prop),                                   \
        .offset    = offsetof(_state, _field)                    \
            + type_check(_type, typeof_field(_state, _field)),   \
        }

#define DEFINE_PROP_SIGNED(_name, _state, _field, _defval, _prop, _type) { \
        .name      = (_name),                                           \
        .info      = &(_prop),                                          \
        .offset    = offsetof(_state, _field)                           \
            + type_check(_type,typeof_field(_state, _field)),           \
        .set_default = true,                                            \
        .defval.i  = (_type)_defval,                                    \
        }

#define DEFINE_PROP_SIGNED_NODEFAULT(_name, _state, _field, _prop, _type) { \
        .name      = (_name),                                           \
        .info      = &(_prop),                                          \
        .offset    = offsetof(_state, _field)                           \
            + type_check(_type, typeof_field(_state, _field)),          \
        }

#define DEFINE_PROP_BIT(_name, _state, _field, _bit, _defval) {  \
        .name      = (_name),                                    \
        .info      = &(qdev_prop_bit),                           \
        .bitnr    = (_bit),                                      \
        .offset    = offsetof(_state, _field)                    \
            + type_check(uint32_t,typeof_field(_state, _field)), \
        .set_default = true,                                     \
        .defval.u  = (bool)_defval,                              \
        }

#define DEFINE_PROP_UNSIGNED(_name, _state, _field, _defval, _prop, _type) { \
        .name      = (_name),                                           \
        .info      = &(_prop),                                          \
        .offset    = offsetof(_state, _field)                           \
            + type_check(_type, typeof_field(_state, _field)),          \
        .set_default = true,                                            \
        .defval.u  = (_type)_defval,                                    \
        }

#define DEFINE_PROP_UNSIGNED_NODEFAULT(_name, _state, _field, _prop, _type) { \
        .name      = (_name),                                           \
        .info      = &(_prop),                                          \
        .offset    = offsetof(_state, _field)                           \
            + type_check(_type, typeof_field(_state, _field)),          \
        }

#define DEFINE_PROP_BIT64(_name, _state, _field, _bit, _defval) {       \
        .name      = (_name),                                           \
        .info      = &(qdev_prop_bit64),                                \
        .bitnr    = (_bit),                                             \
        .offset    = offsetof(_state, _field)                           \
            + type_check(uint64_t, typeof_field(_state, _field)),       \
        .set_default = true,                                            \
        .defval.u  = (bool)_defval,                                     \
        }

#define DEFINE_PROP_BOOL(_name, _state, _field, _defval) {       \
        .name      = (_name),                                    \
        .info      = &(qdev_prop_bool),                          \
        .offset    = offsetof(_state, _field)                    \
            + type_check(bool, typeof_field(_state, _field)),    \
        .set_default = true,                                     \
        .defval.u    = (bool)_defval,                            \
        }

#define PROP_ARRAY_LEN_PREFIX "len-"

/**
 * DEFINE_PROP_ARRAY:
 * @_name: name of the array
 * @_state: name of the device state structure type
 * @_field: uint32_t field in @_state to hold the array length
 * @_arrayfield: field in @_state (of type '@_arraytype *') which
 *               will point to the array
 * @_arrayprop: PropertyInfo defining what property the array elements have
 * @_arraytype: C type of the array elements
 *
 * Define device properties for a variable-length array _name.  A
 * static property "len-arrayname" is defined. When the device creator
 * sets this property to the desired length of array, further dynamic
 * properties "arrayname[0]", "arrayname[1]", ...  are defined so the
 * device creator can set the array element values. Setting the
 * "len-arrayname" property more than once is an error.
 *
 * When the array length is set, the @_field member of the device
 * struct is set to the array length, and @_arrayfield is set to point
 * to (zero-initialised) memory allocated for the array.  For a zero
 * length array, @_field will be set to 0 and @_arrayfield to NULL.
 * It is the responsibility of the device deinit code to free the
 * @_arrayfield memory.
 */
#define DEFINE_PROP_ARRAY(_name, _state, _field,                        \
                          _arrayfield, _arrayprop, _arraytype) {        \
        .name = (PROP_ARRAY_LEN_PREFIX _name),                          \
        .info = &(qdev_prop_arraylen),                                  \
        .set_default = true,                                            \
        .defval.u = 0,                                                  \
        .offset = offsetof(_state, _field)                              \
            + type_check(uint32_t, typeof_field(_state, _field)),       \
        .arrayinfo = &(_arrayprop),                                     \
        .arrayfieldsize = sizeof(_arraytype),                           \
        .arrayoffset = offsetof(_state, _arrayfield),                   \
        }

#define DEFINE_PROP_LINK(_name, _state, _field, _type, _ptr_type) {     \
        .name = (_name),                                                \
        .info = &(qdev_prop_link),                                      \
        .offset = offsetof(_state, _field)                              \
            + type_check(_ptr_type, typeof_field(_state, _field)),      \
        .link_type  = _type,                                            \
        }

#define DEFINE_PROP_UINT8(_n, _s, _f, _d)                       \
    DEFINE_PROP_UNSIGNED(_n, _s, _f, _d, qdev_prop_uint8, uint8_t)
#define DEFINE_PROP_UINT16(_n, _s, _f, _d)                      \
    DEFINE_PROP_UNSIGNED(_n, _s, _f, _d, qdev_prop_uint16, uint16_t)
#define DEFINE_PROP_UINT32(_n, _s, _f, _d)                      \
    DEFINE_PROP_UNSIGNED(_n, _s, _f, _d, qdev_prop_uint32, uint32_t)
#define DEFINE_PROP_INT32(_n, _s, _f, _d)                      \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_int32, int32_t)
#define DEFINE_PROP_UINT64(_n, _s, _f, _d)                      \
    DEFINE_PROP_UNSIGNED(_n, _s, _f, _d, qdev_prop_uint64, uint64_t)
#define DEFINE_PROP_INT64(_n, _s, _f, _d)                      \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_int64, int64_t)
#define DEFINE_PROP_SIZE(_n, _s, _f, _d)                       \
    DEFINE_PROP_UNSIGNED(_n, _s, _f, _d, qdev_prop_size, uint64_t)
#define DEFINE_PROP_PCI_DEVFN(_n, _s, _f, _d)                   \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_pci_devfn, int32_t)

#define DEFINE_PROP_CHR(_n, _s, _f)             \
    DEFINE_PROP(_n, _s, _f, qdev_prop_chr, CharBackend)
#define DEFINE_PROP_STRING(_n, _s, _f)             \
    DEFINE_PROP(_n, _s, _f, qdev_prop_string, char*)
#define DEFINE_PROP_NETDEV(_n, _s, _f)             \
    DEFINE_PROP(_n, _s, _f, qdev_prop_netdev, NICPeers)
#define DEFINE_PROP_DRIVE(_n, _s, _f) \
    DEFINE_PROP(_n, _s, _f, qdev_prop_drive, BlockBackend *)
#define DEFINE_PROP_DRIVE_IOTHREAD(_n, _s, _f) \
    DEFINE_PROP(_n, _s, _f, qdev_prop_drive_iothread, BlockBackend *)
#define DEFINE_PROP_MACADDR(_n, _s, _f)         \
    DEFINE_PROP(_n, _s, _f, qdev_prop_macaddr, MACAddr)
#define DEFINE_PROP_RESERVED_REGION(_n, _s, _f)         \
    DEFINE_PROP(_n, _s, _f, qdev_prop_reserved_region, ReservedRegion)
#define DEFINE_PROP_ON_OFF_AUTO(_n, _s, _f, _d) \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_on_off_auto, OnOffAuto)
#define DEFINE_PROP_MULTIFD_COMPRESSION(_n, _s, _f, _d) \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_multifd_compression, \
                       MultiFDCompression)
#define DEFINE_PROP_LOSTTICKPOLICY(_n, _s, _f, _d) \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_losttickpolicy, \
                        LostTickPolicy)
#define DEFINE_PROP_BLOCKDEV_ON_ERROR(_n, _s, _f, _d) \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_blockdev_on_error, \
                        BlockdevOnError)
#define DEFINE_PROP_BIOS_CHS_TRANS(_n, _s, _f, _d) \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_bios_chs_trans, int)
#define DEFINE_PROP_SIZE32(_n, _s, _f, _d)                       \
    DEFINE_PROP_UNSIGNED(_n, _s, _f, _d, qdev_prop_size32, uint32_t)
#define DEFINE_PROP_BLOCKSIZE(_n, _s, _f) \
    DEFINE_PROP_UNSIGNED(_n, _s, _f, 0, qdev_prop_blocksize, uint32_t)
#define DEFINE_PROP_PCI_HOST_DEVADDR(_n, _s, _f) \
    DEFINE_PROP(_n, _s, _f, qdev_prop_pci_host_devaddr, PCIHostDeviceAddress)
#define DEFINE_PROP_OFF_AUTO_PCIBAR(_n, _s, _f, _d) \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_off_auto_pcibar, \
                        OffAutoPCIBAR)
#define DEFINE_PROP_PCIE_LINK_SPEED(_n, _s, _f, _d) \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_pcie_link_speed, \
                        PCIExpLinkSpeed)
#define DEFINE_PROP_PCIE_LINK_WIDTH(_n, _s, _f, _d) \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_pcie_link_width, \
                        PCIExpLinkWidth)

#define DEFINE_PROP_UUID(_name, _state, _field) {                  \
        .name      = (_name),                                      \
        .info      = &qdev_prop_uuid,                              \
        .offset    = offsetof(_state, _field)                      \
            + type_check(QemuUUID, typeof_field(_state, _field)),  \
        .set_default = true,                                       \
        }
#define DEFINE_PROP_AUDIODEV(_n, _s, _f) \
    DEFINE_PROP(_n, _s, _f, qdev_prop_audiodev, QEMUSoundCard)

#define DEFINE_PROP_UUID_NODEFAULT(_name, _state, _field) {        \
        .name      = (_name),                                      \
        .info      = &qdev_prop_uuid,                              \
        .offset    = offsetof(_state, _field)                      \
            + type_check(QemuUUID, typeof_field(_state, _field)),  \
        }

#define DEFINE_PROP_END_OF_LIST()               \
    {}

/**
 * object_class_property_add_static: Add a static property to object class
 * @oc: object class
 * @prop: property definition
 * @allow_set: optional check function
 *
 * Add a property to an object class based on the property definition
 * at @prop.
 *
 * If @allow_set is NULL, the property will always be allowed to be set.
 *
 * The property definition at @prop should be defined using the
 * ``DEFINE_PROP`` family of macros.  *@prop must exist for the
 * life time of @oc.
 */
ObjectProperty *
object_class_property_add_static(ObjectClass *oc, Property *prop,
                                 ObjectPropertyAllowSet allow_set);

/**
 * object_class_add_static_props: Add multiple static properties to object class
 * @oc: object class
 * @props: property definition array, terminated by DEFINED_PROP_END_OF_LIST()
 * @allow_set: optional check function
 *
 * Add properties from @props using object_class_property_add_static()
 */
void object_class_add_static_props(ObjectClass *oc, Property *props,
                                   ObjectPropertyAllowSet allow_set);

/*
 * Set properties between creation and realization.
 *
 * Returns: %true on success, %false on error.
 */
bool qdev_prop_set_drive_err(DeviceState *dev, const char *name,
                             BlockBackend *value, Error **errp);

/*
 * Set properties between creation and realization.
 * @value must be valid.  Each property may be set at most once.
 */
void qdev_prop_set_bit(DeviceState *dev, const char *name, bool value);
void qdev_prop_set_uint8(DeviceState *dev, const char *name, uint8_t value);
void qdev_prop_set_uint16(DeviceState *dev, const char *name, uint16_t value);
void qdev_prop_set_uint32(DeviceState *dev, const char *name, uint32_t value);
void qdev_prop_set_int32(DeviceState *dev, const char *name, int32_t value);
void qdev_prop_set_uint64(DeviceState *dev, const char *name, uint64_t value);
void qdev_prop_set_string(DeviceState *dev, const char *name, const char *value);
void qdev_prop_set_chr(DeviceState *dev, const char *name, Chardev *value);
void qdev_prop_set_netdev(DeviceState *dev, const char *name, NetClientState *value);
void qdev_prop_set_drive(DeviceState *dev, const char *name,
                         BlockBackend *value);
void qdev_prop_set_macaddr(DeviceState *dev, const char *name,
                           const uint8_t *value);
void qdev_prop_set_enum(DeviceState *dev, const char *name, int value);

void *object_static_prop_ptr(Object *obj, Property *prop);

void qdev_prop_register_global(GlobalProperty *prop);
const GlobalProperty *qdev_find_global_prop(Object *obj,
                                            const char *name);
int qdev_prop_check_globals(void);
void qdev_prop_set_globals(DeviceState *dev);
void error_set_from_qdev_prop_error(Error **errp, int ret, Object *obj,
                                    Property *prop, const char *value);

/**
 * qdev_property_add_static:
 * @dev: Device to add the property to.
 * @prop: The qdev property definition.
 *
 * Add a static QOM property to @dev for qdev property @prop.
 * On error, store error in @errp.  Static properties access data in a struct.
 * The type of the QOM property is derived from prop->info.
 */
void qdev_property_add_static(DeviceState *dev, Property *prop);

/**
 * qdev_alias_all_properties: Create aliases on source for all target properties
 * @target: Device which has properties to be aliased
 * @source: Object to add alias properties to
 *
 * Add alias properties to the @source object for all qdev properties on
 * the @target DeviceState.
 *
 * This is useful when @target is an internal implementation object
 * owned by @source, and you want to expose all the properties of that
 * implementation object as properties on the @source object so that users
 * of @source can set them.
 */
void qdev_alias_all_properties(DeviceState *target, Object *source);

/**
 * @qdev_prop_set_after_realize:
 * @dev: device
 * @name: name of property
 * @errp: indirect pointer to Error to be set
 * Set the Error object to report that an attempt was made to set a property
 * on a device after it has already been realized. This is a utility function
 * which allows property-setter functions to easily report the error in
 * a friendly format identifying both the device and the property.
 */
void qdev_prop_set_after_realize(DeviceState *dev, const char *name,
                                 Error **errp);

/**
 * qdev_prop_allow_set_link_before_realize:
 *
 * Set the #Error object if an attempt is made to set the link after realize.
 * This function should be used as the check() argument to
 * object_property_add_link().
 */
void qdev_prop_allow_set_link_before_realize(const Object *obj,
                                             const char *name,
                                             Object *val, Error **errp);

#endif
