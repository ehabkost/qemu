#ifndef HW_QDEV_PROPERTIES_SYSTEM_H
#define HW_QDEV_PROPERTIES_SYSTEM_H

#include "hw/qdev-properties.h"

extern const PropertyInfo qdev_prop_drive;
extern const PropertyInfo qdev_prop_drive_iothread;
extern const PropertyInfo qdev_prop_chr;
extern const PropertyInfo qdev_prop_macaddr;
extern const PropertyInfo qdev_prop_netdev;
extern const PropertyInfo qdev_prop_audiodev;
extern const PropertyInfo qdev_prop_losttickpolicy;
extern const PropertyInfo qdev_prop_blocksize;
extern const PropertyInfo qdev_prop_blockdev_on_error;
extern const PropertyInfo qdev_prop_bios_chs_trans;
extern const PropertyInfo qdev_prop_fdc_drive_type;
extern const PropertyInfo qdev_prop_multifd_compression;
extern const PropertyInfo qdev_prop_reserved_region;
extern const PropertyInfo qdev_prop_pci_devfn;
extern const PropertyInfo qdev_prop_pci_host_devaddr;
extern const PropertyInfo qdev_prop_off_auto_pcibar;
extern const PropertyInfo qdev_prop_pcie_link_speed;
extern const PropertyInfo qdev_prop_pcie_link_width;

#define DEFINE_PROP_PCI_DEVFN(_n, _s, _f, _d)                   \
    DEFINE_PROP_SIGNED(_n, _s, _f, _d, qdev_prop_pci_devfn, int32_t)
#define DEFINE_PROP_CHR(_n, _s, _f)             \
    DEFINE_PROP(_n, _s, _f, qdev_prop_chr, CharBackend)
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
#define DEFINE_PROP_AUDIODEV(_n, _s, _f) \
    DEFINE_PROP(_n, _s, _f, qdev_prop_audiodev, QEMUSoundCard)



#endif
