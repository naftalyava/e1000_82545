#ifndef _i82545_H_
#define _i82545_H_

char i82545_driver_name[] = "82545EM - Test Driver";
static char i82545_driver_string[] = "82545EM NIC Driver";
static const char i82545_copyright[] = "Copyright (c) 2021 Naftaly Avadiaev.";

#define PCI_VENDOR_ID_INTEL 0x8086
#define PCI_DEVICE_ID_82545 0x100f

#define BAR0 0
#define BAR1 1






static int i82545_init_module(void);
static void i82545_exit_module(void);

static int probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void remove(struct pci_dev *pdev);
static void shutdown(struct pci_dev *pdev);

static int suspend(struct device *dev);
static int resume(struct device *dev);


#endif
