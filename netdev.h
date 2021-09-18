#include <linux/netdevice.h>



void netdev_open(struct net_device *dev);
int netdev_release(struct net_device *dev);
int netdev_config(struct net_device *dev, struct ifmap *map);
int netdev_tx(struct sk_buff *skb, struct net_device *dev);
