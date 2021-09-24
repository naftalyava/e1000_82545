#define CTRL     0x00000	/* Device Control - RW */
#define FCAL     0x00028	/* Flow Control Address Low - RW */
#define FCAH     0x0002C	/* Flow Control Address High -RW */
#define FCT      0x00030	/* Flow Control Type - RW */
#define FCTTV    0x00170	/* Flow Control Transmit Timer Value - RW */


/* Register Masks */
#define CTRL_RST      0x04000000	/* Global reset */
#define CTRL_ASDE     0x00000020	/* Auto-speed detect enable */
#define CTRL_SLU      0x00000040	/* Set link up (Force Link) */
#define CTRL_ILOS     0x00000080	/* Invert Loss-Of Signal */
#define CTRL_FRCSPD   0x00000800	/* Force Speed */
#define CTRL_FRCDPLX  0x00001000	/* Force Duplex */
#define CTRL_LRST     0x00000008	/* Link reset. 0=normal,1=reset */
#define CTRL_PHY_RST  0x80000000	/* PHY Reset */
#define CTRL_VME      0x40000000	/* IEEE VLAN mode enable */


#define NUM_MTA_REGISTERS 128       /* Number of entries in the Multicast Table Array (MTA). */
#define MTA               0x05200	/* Multicast Table Array - RW Array */



#define reg_write(hw_addr, reg, value)(writel((value), ( (hw_addr) + reg)))
#define reg_read(hw_addr, reg) (readl((hw_addr) + reg))