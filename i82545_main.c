#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/etherdevice.h>
#include "i82545.h"
#include "netdev.h"



static const struct pci_device_id pci_tbl[] = {
	{PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_82545)},
	/* required last entry */
	{0,}
};

MODULE_DEVICE_TABLE(pci, pci_tbl);

SIMPLE_DEV_PM_OPS(i82545_pm_ops, suspend, resume);

static struct pci_driver i82545_driver = {
	.name     = i82545_driver_name,
	.id_table = pci_tbl,
	.probe    = probe,
	.remove   = remove,
	.driver.pm = &i82545_pm_ops,
	.shutdown = shutdown,
};

struct adapter {
	struct net_device_stats stats;
	struct pci_dev *pdev;
	struct net_device *netdev;
	u8 __iomem *hw_addr;
	unsigned long io_base;
	int bars;
	unsigned long flags;
	char name[256];
	unsigned int min_mtu;
	unsigned int max_mtu;
};


static const struct net_device_ops nps_netdev_ops = {
	.ndo_open               = ndo_open,
	.ndo_stop               = ndo_stop,
	.ndo_start_xmit         = ndo_start_xmit,
	.ndo_set_mac_address    = ndo_set_mac_address,
	.ndo_set_rx_mode        = ndo_set_rx_mode,
	.ndo_get_stats          = ndo_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= ndo_poll_controller,
#endif
};

static int g_bars;

/*
 * PCI related methods
 */

static int probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct net_device *netdev = NULL;
	struct adapter *adapter = NULL;
	int bars, retval = 0, i;

	pr_info("probe\n");

	if (!pdev){
		pr_err("pdev == NULL!\n");
		goto probe_error;
	}

	/* 
	 * The Linux kernel assigns special memory regions “Base Address Registers” (BARs) to communicate with the hardware.
	 * These memory addresses (addr + length) are written to the PCI controller hardware during the system boot.
	 */
	bars = pci_select_bars(pdev, IORESOURCE_MEM | IORESOURCE_IO);
	g_bars = bars;

	retval = pci_enable_device(pdev);
	if (retval) {
		pr_err("pci_enable_device\n");
		return retval;
	}

	/* Reserve selected PCI I/O and memory resources */
	retval = pci_request_selected_regions(pdev, bars, i82545_driver_name);
	if (retval){
		pr_err("pci_request_selected_regions\n");
		goto probe_error;
	}

	/* This allows us to perform DMA */
	pci_set_master(pdev);

	retval = pci_save_state(pdev);
	if (retval) {
		pr_err("pci_save_state\n");
		goto probe_error;
	}

	netdev = alloc_netdev(sizeof(struct adapter), "net:%d", NET_NAME_ENUM, netdev_open);
	if (!netdev){
		pr_err("alloc_netdev\n");
		goto probe_error;
	}

	netdev->netdev_ops = &nps_netdev_ops;


		


	SET_NETDEV_DEV(netdev, &pdev->dev);
	pci_set_drvdata(pdev, netdev);

	adapter = netdev_priv(netdev);
	adapter->netdev = netdev;
	adapter->pdev = pdev;
	adapter->bars = bars;

	adapter->hw_addr = pci_ioremap_bar(pdev, BAR0);
	if (!adapter->hw_addr){ 
		pr_err("pci_ioremap_bar\n");
		goto probe_error;
	}

	for (i = BAR1; i < PCI_STD_NUM_BARS; i++) {
		if (pci_resource_len(pdev, i) == 0)
			continue;
		if (pci_resource_flags(pdev, i) & IORESOURCE_IO) {
			adapter->io_base = pci_resource_start(pdev, i);
			break;
		}
	}




	// // adapter->min_mtu = 46;
	// // adapter->max_mtu = 1540;





	//strcpy(netdev->name, "eth%d");
	//pr_info("netdev->name: %s\n", netdev->name);
	retval = register_netdev(netdev);
	if (retval) {
		pr_err("register_netdev\n");
		goto probe_error;
	}

	pr_info("probe: all good!\n");
		



probe_error:
	return retval;
}

static void remove(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct adapter *adapter = netdev_priv(netdev);

	

	pci_release_selected_regions(pdev, adapter->bars);

	// why we crash here?
	// unregister_netdev(netdev);

	// iounmap(hw_addr);


	// pci_release_selected_regions(pdev, bars);
	// pci_disable_device(pdev);
	// free_netdev(netdev);
	pci_disable_device(pdev);
}

static void shutdown(struct pci_dev *pdev)
{

}


/*
 * Device related methods
 */

static int suspend(struct device *dev)
{
	int retval = 0;
	return retval;
}

static int resume(struct device *dev)
{
	int retval = 0;
	return retval;
}



/*
 * Main driver init and driver exit modules
 */

static int __init i82545_init_module(void)
{
	int ret;
	pr_info("%s: i82545_init_module\n", i82545_driver_string);
	ret = pci_register_driver(&i82545_driver);

	if (ret)
		pr_err("%s failed to load\n", i82545_driver_string);

	pr_info("%s: i82545_init_module ok\n", i82545_driver_string);
	return ret;
}

static void __exit i82545_exit_module(void)
{
	pr_info("%s: i82545_exit_module\n", i82545_driver_string);
	pci_unregister_driver(&i82545_driver);
}

module_init(i82545_init_module);
module_exit(i82545_exit_module);


/*
 * Misc
 */

MODULE_AUTHOR("Naftaly Avadiaev");
MODULE_DESCRIPTION("Test Driver for Intel Gigabit 82545EN");
MODULE_LICENSE("GPL v2");
