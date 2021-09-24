#include "netdev.h"


static irqreturn_t interrupt(int irq, void *dev_id)
{
    pr_info("interrupt\n");
    return IRQ_HANDLED;
}


void netdev_open(struct net_device *netdev)
{
    int retval = 0;

    struct adapter *adapter = netdev_priv(netdev);

    pr_info("netdev_open\n");

    netif_carrier_on(netdev);
    
    if (netif_carrier_ok(netdev)){
        pr_info("netif_carrier_ok\n");
    }

    //netif_start_queue(netdev);

    // if ((retval = request_irq(adapter->pdev->irq, interrupt, IRQF_SHARED, adapter->netdev->name, adapter->netdev))) {
    //     pr_info("netdev_open: register interrupt failed!\n");
    // }

    //pr_info("netdev_open: register interrupt succeded!\n");

    return;
}

int netdev_release(struct net_device *dev)
{
    int retval = 0;
    return retval;
}

int netdev_config(struct net_device *dev, struct ifmap *map)
{
    int retval = 0;
    return retval;
}

int netdev_tx(struct sk_buff *skb, struct net_device *dev)
{
    int retval = 0;
    return retval;
}


int ndo_open(struct net_device *netdev)
{
    int retval = 0;

    pr_info("netdev_open\n");


    netif_carrier_on(netdev);

    netif_start_queue(netdev);

    return retval;
}

int ndo_stop(struct net_device *netdev)
{
    int retval = 0;

    netif_stop_queue(netdev);

    return retval;
}

netdev_tx_t ndo_start_xmit(struct sk_buff *skb, struct net_device *netdev)
{
    netdev_tx_t retval = NETDEV_TX_OK;
    return retval;
}

s32 ndo_set_mac_address(struct net_device *ndev, void *p)
{
    s32 retval = 0;
    return retval;
}

void ndo_set_rx_mode(struct net_device *netdev)
{
    return;
}

void ndo_poll_controller(struct net_device *ndev)
{

}

struct net_device_stats* ndo_get_stats(struct net_device *dev)
{
    struct adapter *adapter = netdev_priv(dev);
    return &adapter->stats;

    

    return NULL;
}