#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

char driver_name[] = "82545EM - Test Driver";
static char driver_string[] = "Intel(R) 82545EM NIC Driver";
static const char copyright[] = "Copyright (c) 2021 Naftaly Avadiaev.";


#define PCI_VENDOR_ID_INTEL 0x8086
#define PCI_DEVICE_ID_82545 0x100f




static const struct pci_device_id pci_tbl[] = {
	{PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_82545)},
	/* required last entry */
	{0,}
};

MODULE_DEVICE_TABLE(pci, pci_tbl);






static int e1000_82545_init_module(void);
static void e1000_82545_exit_module(void);

static struct pci_driver driver = {
	.name     = driver_name,
	.id_table = pci_tbl,
	.probe    = NULL,
	.remove   = NULL,
	.driver = {
		.pm = NULL,
	},
	.shutdown = NULL,
};



static int __init e1000_82545_init_module(void)
{
	int ret;
	pr_info("%s\n", driver_string);
	pr_info("%s\n", copyright);
	ret = pci_register_driver(&driver);
	return ret;
}



/**
 * e1000_exit_module - Driver Exit Cleanup Routine
 *
 * e1000_exit_module is called just before the driver is removed
 * from memory.
 **/
static void __exit e1000_82545_exit_module(void)
{
	pci_unregister_driver(&driver);
}


module_init(e1000_82545_init_module);
module_exit(e1000_82545_exit_module);