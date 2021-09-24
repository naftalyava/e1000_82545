#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/etherdevice.h>
#include <linux/io.h>
#include "i82545.h"
#include "netdev.h"
#include "hw.h"



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



static const struct net_device_ops nps_netdev_ops = {
	.ndo_open               = ndo_open,
	.ndo_stop               = ndo_stop,
	.ndo_start_xmit         = ndo_start_xmit,
	.ndo_set_mac_address    = eth_mac_addr, //ndo_set_mac_address,
	.ndo_set_rx_mode        = ndo_set_rx_mode,
	.ndo_get_stats          = ndo_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= ndo_poll_controller,
#endif
};


/*
 * -------------------- Hardware related methods ------------------------------
 */


/*
    1. Internal PHY mode - auto speed & duplex detection
    2. Link Reset

 */
void init_general(struct adapter *adapter)
{
	u32 ctrl, fcal, fcah, fct, fcttv;

	// reset hw
	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | CTRL_RST);
	

	// auto speed & duplex detection enable
	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | CTRL_ASDE);

	//verify 
	ctrl = reg_read(adapter->hw_addr, CTRL);
	pr_info("CTRL_ASDE: %d", (ctrl & CTRL_ASDE) ? 1:0);


	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | ~CTRL_FRCSPD);

	//verify 
	ctrl = reg_read(adapter->hw_addr, CTRL);
	pr_info("CTRL_FRCSPD: %d", (ctrl & CTRL_FRCSPD) ? 1:0);


	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | ~CTRL_FRCDPLX);

	//verify 
	ctrl = reg_read(adapter->hw_addr, CTRL);
	pr_info("CTRL_FRCDPLX: %d", (ctrl & CTRL_FRCDPLX) ? 1:0);


	// setup link up
	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | CTRL_SLU);

	//verify 
	ctrl = reg_read(adapter->hw_addr, CTRL);
	pr_info("CTRL_SLU: %d", (ctrl & CTRL_SLU) ? 1:0);


	//link reset
	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | ~CTRL_LRST);

	//verify 
	ctrl = reg_read(adapter->hw_addr, CTRL);
	pr_info("CTRL_LRST: %d", (ctrl & CTRL_LRST) ? 1:0);


	//phy reset
	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | ~CTRL_PHY_RST);

	//verify 
	ctrl = reg_read(adapter->hw_addr, CTRL);
	pr_info("CTRL_PHY_RST: %d", (ctrl & CTRL_PHY_RST) ? 1:0);


	//ilos
	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | ~CTRL_ILOS);

	//verify 
	ctrl = reg_read(adapter->hw_addr, CTRL);
	pr_info("CTRL_ILOS: %d", (ctrl & CTRL_ILOS) ? 1:0);


	//fc
	fcal = reg_read(adapter->hw_addr, FCAL);
	reg_write(adapter->hw_addr, FCAL, 0);

	//verify 
	fcal = reg_read(adapter->hw_addr, FCAL);
	pr_info("FCAL: %d", (fcal & 0) ? 1:0);


	fcah = reg_read(adapter->hw_addr, FCAH);
	reg_write(adapter->hw_addr, FCAH, 0);

	//verify 
	fcah = reg_read(adapter->hw_addr, FCAH);
	pr_info("FCAH: %d", (fcah & 0) ? 1:0);


	fct = reg_read(adapter->hw_addr, FCT);
	reg_write(adapter->hw_addr, FCT, 0);

	//verify 
	fct = reg_read(adapter->hw_addr, FCT);
	pr_info("FCT: %d", (fct & 0) ? 1:0);


	fcttv = reg_read(adapter->hw_addr, FCTTV);
	reg_write(adapter->hw_addr, FCTTV, 0);

	//verify 
	fcttv = reg_read(adapter->hw_addr, FCTTV);
	pr_info("FCTTV: %d", (fcttv & 0) ? 1:0);


	//vlan
	ctrl = reg_read(adapter->hw_addr, CTRL);
	reg_write(adapter->hw_addr, CTRL, ctrl | ~CTRL_VME);

	//verify 
	ctrl = reg_read(adapter->hw_addr, CTRL);
	pr_info("CTRL_VME: %d", (ctrl & CTRL_VME) ? 1:0);


	


}

/*
 * ----------------------------------------------------------------------------
 */




static void watchdog(struct work_struct *work)
{
	struct adapter *adapter = container_of(work, struct adapter, watchdog_task.work);
	struct net_device *netdev = adapter->netdev;
	u32 link;

	link = true;
	if ((netif_carrier_ok(netdev)) && link)
		return;

	if (link) {
		if (!netif_carrier_ok(netdev)) {
			pr_info("%s NIC Link is Up", netdev->name);
			netif_carrier_on(netdev);
		}
	} else {
		if (netif_carrier_ok(netdev)) {
			adapter->link_speed = 0;
			adapter->link_duplex = 0;
			pr_info("%s NIC Link is Down\n", netdev->name);
			netif_carrier_off(netdev);
		}

	}
}








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

	netdev = alloc_netdev(sizeof(struct adapter), "net%d", NET_NAME_ENUM, netdev_open);
	if (!netdev){
		pr_err("alloc_netdev\n");
		goto probe_error;
	}


	ether_setup(netdev);

		


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




	adapter->min_mtu = 46;
	adapter->max_mtu = 1540;






	netdev->netdev_ops = &nps_netdev_ops;
	netdev->base_addr = adapter->io_base;
	netdev->dev_addr[0] = 0;
	netdev->dev_addr[1] = 1;
	netdev->dev_addr[2] = 2;
	netdev->dev_addr[3] = 3;
	netdev->dev_addr[4] = 4;
	netdev->dev_addr[5] = 5;
	netdev->broadcast[0] = 0xff;
	netdev->broadcast[1] = 0xff;
	netdev->broadcast[2] = 0xff;
	netdev->broadcast[3] = 0xff;
	netdev->broadcast[4] = 0xff;
	netdev->broadcast[5] = 0xff;
	netdev->irq = pdev->irq;
	netdev->hard_header_len = 14;

	netdev->mtu = 1540;



	retval = register_netdev(netdev);
	if (retval) {
		pr_err("register_netdev\n");
		goto probe_error;
	}


	INIT_DELAYED_WORK(&adapter->watchdog_task, watchdog);
	pr_info("probe: all good!\n");
		
	//netdev->flags           |= IFF_NOARP;
	netdev->features        |= NETIF_F_HW_CSUM;

	init_general(adapter);
	netif_carrier_on(netdev);

probe_error:
	return retval;
}

static void remove(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct adapter *adapter = netdev_priv(netdev);


	unregister_netdev(netdev);
	pci_release_selected_regions(pdev, adapter->bars);
	iounmap(adapter->hw_addr);
	pci_disable_device(pdev);
	free_netdev(netdev);
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
