#include <linux/netdevice.h>
#include <linux/skbuff.h>



void netdev_open(struct net_device *dev);
int netdev_release(struct net_device *dev);
int netdev_config(struct net_device *dev, struct ifmap *map);
int netdev_tx(struct sk_buff *skb, struct net_device *dev);


int ndo_open(struct net_device *netdev);
int ndo_stop(struct net_device *netdev);
netdev_tx_t ndo_start_xmit(struct sk_buff *skb, struct net_device *netdev);
s32 ndo_set_mac_address(struct net_device *ndev, void *p);
void ndo_set_rx_mode(struct net_device *netdev);
void ndo_poll_controller(struct net_device *ndev);
struct net_device_stats* ndo_get_stats(struct net_device *dev);