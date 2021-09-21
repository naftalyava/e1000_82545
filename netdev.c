#include "netdev.h"




void netdev_open(struct net_device *dev)
{
    pr_info("netdev_open\n");
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
    return retval;
}

int ndo_stop(struct net_device *netdev)
{
    int retval = 0;
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
    return NULL;
}