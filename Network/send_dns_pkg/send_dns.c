#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define DNS_LISTEN_PORT 53
#define MAX_DNS_LEN 512

struct dns_header
{
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t arcount;
}__attribute__((packed));
#define DNS_HEADER_LEN 12

#define MAX_QNAME 255

#define QTYPE_A 1
#define QCLASS_IN 1

struct question_without_qname
{
    uint16_t qtype;
    uint16_t qclass;
}__attribute__((packed));
#define QUESTION_WITHOUT_QNAME_LEN 4

static uint8_t qname_list[2][MAX_QNAME] = 
{
    {0x05, 'm', 'u', 's', 'i', 'c', 0x03, '1', '6', '3', 0x03, 'c', 'o', 'm', 0x00},
    {0x05, 'b', 'a', 'i', 'd', 'u', 0x03, 'c', 'o', 'm', 0x00},
};

static uint8_t dns_package_buf[MAX_DNS_LEN];

int main()
{
    int sock_fp;
    struct sockaddr_in remote_addr;
    struct dns_header * dnsh_ptr = NULL;
    uint8_t * qname = NULL;
    struct question_without_qname * question_ptr = NULL;
    uint16_t dns_send_len = 0;
    
    memset(&remote_addr, 0, sizeof(struct sockaddr_in));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("192.1.1.1");
    remote_addr.sin_port = htons(DNS_LISTEN_PORT);
    
    sock_fp = socket(AF_INET, SOCK_DGRAM, 0);
    
    memset(dns_package_buf, 0, MAX_DNS_LEN);
    
    dnsh_ptr = (struct dns_header *)dns_package_buf;
    dnsh_ptr->id = htons(0x5b02);
    dnsh_ptr->flags = htons(0x0100);
    dnsh_ptr->qdcount = htons(2);
    
    qname = dns_package_buf + DNS_HEADER_LEN;
    memcpy(qname, qname_list[0], strlen(qname_list[0]) + 1);
    
    question_ptr = (struct question_without_qname *)(qname + strlen(qname_list[0]) + 1);
    question_ptr->qtype = htons(QTYPE_A);
    question_ptr->qclass = htons(QCLASS_IN);
    
    
    qname = ((uint8_t *)question_ptr) + QUESTION_WITHOUT_QNAME_LEN;
    memcpy(qname, qname_list[1], strlen(qname_list[1]) + 1);
    
    question_ptr = (struct question_without_qname *)(qname + strlen(qname_list[1]) + 1);
    question_ptr->qtype = htons(QTYPE_A);
    question_ptr->qclass = htons(QCLASS_IN);
    
    
    dns_send_len = ((uint8_t *)question_ptr + QUESTION_WITHOUT_QNAME_LEN) - dns_package_buf;
    sendto(sock_fp, dns_package_buf, dns_send_len, 0, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr_in));
    
    return 0;
}
