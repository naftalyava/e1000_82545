#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include "i82545.h"


static const struct pci_device_id pci_tbl[] = {
	{PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_82545)},
	/* required last entry */
	{0,}
};

MODULE_DEVICE_TABLE(pci, pci_tbl);

SIMPLE_DEV_PM_OPS(i82545_pm_ops, i82545_suspend, i82545_resume);

static struct pci_driver i82545_driver = {
	.name     = i82545_driver_name,
	.id_table = pci_tbl,
	.probe    = NULL,
	.remove   = NULL,
	.driver.pm = &i82545_pm_ops,
	.shutdown = NULL,
};




static int __maybe_unused i82545_suspend(struct device *dev)
{
	int retval = 0;
	return retval;
}


static int __maybe_unused i82545_resume(struct device *dev)
{
	int retval = 0;
	return retval;
}






static int __init i82545_init_module(void)
{
	int ret;
	pr_info("%s\n", i82545_driver_string);
	ret = pci_register_driver(&i82545_driver);

	if (ret)
		pr_err("%s failed to load\n", i82545_driver_string);

	return ret;
}


static void __exit i82545_exit_module(void)
{
	pci_unregister_driver(&i82545_driver);
}


module_init(i82545_init_module);
module_exit(i82545_exit_module);



MODULE_AUTHOR("Naftaly Avadiaev");
MODULE_DESCRIPTION("Test Driver for Intel Gigabit 82545EN");
MODULE_LICENSE("GPL v2");
