#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

#include<linux/netdevice.h>
#include<linux/netfilter.h>
#include<linux/netfilter_ipv4.h>
#include<linux/ip.h>
#include<linux/types.h>
#include<linux/skbuff.h>
#include<linux/if_ether.h>
#include<linux/if_packet.h>
#include<net/tcp.h>
#include<net/udp.h>
#include<net/icmp.h>
#include<linux/inet.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("cch667");
MODULE_DESCRIPTION("Send UDP package through skb.");
MODULE_VERSION("1.0");

#define ETH "eth1"
#define SIP "192.168.56.101"
#define DIP "192.168.56.1"
#define SPORT 39804
#define DPORT 6980

unsigned char SMAC[ETH_ALEN] =
{0x08, 0x00, 0x27, 0xde, 0x29, 0x01}; /* the mac addr of VM ubuntu 12.02 */
unsigned char DMAC[ETH_ALEN] = 
{0x0a, 0x00, 0x27, 0x00, 0x00, 0x11}; /* the mac addr of win7 */

/* The definition of struct iphdr */
/*
struct iphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8	ihl:4,
		version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
	__u8	version:4,
  		ihl:4;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	__u8	tos;
	__be16	tot_len;
	__be16	id;
	__be16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__sum16	check;
	__be32	saddr;
	__be32	daddr;
	// The options start here.
};
*/

static int build_and_xmit_udp(char *eth, u_char *smac, u_char *dmac, u_char *pkt, int pkt_len, u_long sip, u_long dip, u_short sport, u_short dport)
{
    struct sk_buff *skb = NULL;
    struct net_device *dev = NULL;
    struct udphdr *udph = NULL;
    struct iphdr *iph = NULL;
    struct ethhdr *ethdr = NULL;
    u_char *pdata = NULL;
    int nret = 1;

    if(NULL == smac || NULL == dmac)
    {
        goto error_out;
    }

    //根据设备名获得设备指针
    //这里调用的函数高版本做了修改，多了个参数struct net*
    if(NULL == (dev = dev_get_by_name(&init_net, eth)))
    {
        goto error_out;
    }

    //创建一个skb
    skb = alloc_skb(pkt_len + sizeof(struct udphdr) + sizeof(struct iphdr) + sizeof(struct ethhdr), GFP_ATOMIC);
    if(NULL == skb)
    {
        goto error_out;
    }

    //为skb预留空间，方便后面skb_buff协议封装
    skb_reserve(skb, pkt_len + sizeof(struct udphdr) + sizeof(struct iphdr) + sizeof(struct ethhdr));

    //skb字节填充
    skb->dev = dev;
    skb->pkt_type = PACKET_OTHERHOST;
    skb->protocol = __constant_htons(ETH_P_IP);
    skb->ip_summed = CHECKSUM_NONE;
    skb->priority = 0;

    //数据包封装
    //分别压入应用层，传输层，网络层，链路层栈帧
    //skb_push由后面往前面，与skb_put不同
    pdata = skb_push(skb, pkt_len);
    udph = (struct udphdr*)skb_push(skb, sizeof(struct udphdr));
    iph = (struct iphdr*)skb_push(skb, sizeof(struct iphdr));
    ethdr = (struct ethhdr*)skb_push(skb, sizeof(struct ethhdr));

    //应用层数据填充
    memcpy(pdata, pkt, pkt_len);

    //传输层udp数据填充
    memset(udph, 0, sizeof(struct udphdr));
    udph->source = sport;
    udph->dest = dport;
    udph->len = htons(sizeof(struct udphdr) + pkt_len);//主机字节序转网络字节序
    udph->check = 0;//skb_checksum之前必须置0.协议规定

    //网络层数据填充
    iph->version = 4;
    iph->ihl = sizeof(struct iphdr) >> 2;
    iph->frag_off = 0;
    iph->protocol = IPPROTO_UDP;
    iph->tos = 0;
    iph->daddr = dip;
    iph->saddr = sip;
    iph->ttl = 0x40;
    iph->tot_len = __constant_htons(skb->len - sizeof(struct ethhdr));
    iph->check = 0;
    iph->check = ip_fast_csum((unsigned char*)iph, iph->ihl);//计算校验和

    skb->csum = skb_checksum(skb, iph->ihl*4, skb->len - iph->ihl*4, 0);//skb校验和计算
    udph->check = csum_tcpudp_magic(sip, dip, skb->len - iph->ihl*4, IPPROTO_UDP, skb->csum);//dup和tcp伪首部校验和

    //链路层数据填充
    memcpy(ethdr->h_dest, dmac, ETH_ALEN);
    memcpy(ethdr->h_source, smac, ETH_ALEN);
    ethdr->h_proto = __constant_htons(ETH_P_IP);

    //调用内核协议栈函数，发送数据包
    if(dev_queue_xmit(skb) < 0)
    {
        printk("dev_queue_xmit error\n");
        goto error_out;
    }
    nret = 0;//这里是必须的
    printk("dev_queue_xmit correct\n");
    //出错处理
error_out:
    /*下面的0!=nret是必须的，前面即使不执行goto error_out，下面的语句程序也会执行，
    如果不加0!=nret语句，那么前面dev_queue_xmit返回之后（已经kfree_skb一次了），
    再进入下面的语句第二次执行kfree_skb，就会导致系统死机*/
    //关键在于知道dev_queue_xmit内部调用成功后，会kfree_skb，以及goto语句的作用

    if(0 != nret && NULL != skb)//这里前面的nret判断是必须的，不然必定死机
    {
        dev_put(dev);//减少设备的引用计数
        kfree_skb(skb);//销毁数据包
    }

    return nret;//F_ACCEPT;
}

static void send_udp_package(void)
{
    int ret = NF_ACCEPT;
    unsigned char *pdata = "hello kernel!";
    
    printk(KERN_INFO "Sending the udp packet\n");
    ret = build_and_xmit_udp(ETH, SMAC, DMAC, pdata, strlen(pdata), in_aton(SIP), in_aton(DIP), htons(SPORT), htons(DPORT));
}

static int __init skb_send_dns_init(void)
{
    printk("@@ skb_send_dns_init!\n");
    
    send_udp_package();
    
    return 0;
}

static void __exit skb_send_dns_exit(void)
{
    printk("@@ skb_send_dns_exit!\n");
}

module_init(skb_send_dns_init);
module_exit(skb_send_dns_exit);
