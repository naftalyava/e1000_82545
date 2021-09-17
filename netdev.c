#include "netdev.h"

void netdev_open(struct net_device *dev)
{
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
