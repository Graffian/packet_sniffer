#include <stdio.h>
#include <pcap.h>
#include <stdint.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

struct ethernet_header{
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
};

struct ip_header{
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dest_ip;
};

struct tcp_header{
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t data_offset;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_ptr;
};

struct udp_header{
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
};

void packet_handler(u_char *args , const struct pcap_pkthdr *header , const u_char *packet){
    struct ethernet_header *eth = (struct ethernet_header *)packet;
    uint16_t ethertype = ntohs(eth->ethertype);

    if (ethertype!= 0x0800){
        printf("Non-IPv4 packet (ethertype 0x%04x), skipping details\n", ethertype);
        return;
    }
    struct ip_header *ip = (struct ip_header *)(packet + 14);
    int ip_header_len = (ip->version_ihl & 0x0F)*4;

    struct in_addr src_addr, dst_addr;
    src_addr.s_addr = ip->src_ip;
    dst_addr.s_addr = ip->dest_ip;
    printf("[%d bytes] %s -> %s  ", header->len, inet_ntoa(src_addr), inet_ntoa(dst_addr));
    const u_char *payload = packet+14+ip_header_len;

    if (ip->protocol == 6){
        struct tcp_header *tcp = (struct tcp_header *)payload;
        printf("TCP  %d -> %d\n", ntohs(tcp->src_port), ntohs(tcp->dest_port));
    }else if(ip->protocol == 17){
        struct udp_header *udp = (struct udp_header *)payload;
        printf("UDP  %d -> %d\n", ntohs(udp->src_port), ntohs(udp->dest_port));
    }else if(ip->protocol == 1){
        printf("ICMP\n");
    }else{
        printf("Other protocol (%d)\n", ip->protocol);
    }

}

int main(){
    char errbuf[PCAP_ERRBUF_SIZE];
    char *device = "\\Device\\NPF_{D8DB7F83-6245-45CB-A9FA-E6D96F45D878}"; /* Intel Wi-Fi 6E AX211 */
    pcap_t *handle = pcap_open_live(
        device , 65535 , 1 , 1000 , errbuf
    );
    if (handle == NULL){
        fprintf(stderr , "Could not open device %s \n" , errbuf);
        return 1;

    }
    struct bpf_program fp;
    char filter_exp[] = "tcp or udp";
    bpf_u_int32 net = 0;

    if(pcap_compile (handle , &fp , filter_exp , 0 , net) == -1){
        fprintf(stderr, "Bad filter: %s\n", pcap_geterr(handle));
        return 1;
    }
    if (pcap_setfilter(handle , &fp) == -1){
        fprintf(stderr , "Could not set filter %s\n" , pcap_geterr(handle));
        return 1;
    }

    printf("Listening on device.. Press Ctrl+C to exit \n\n");
    pcap_loop(handle , -1 , packet_handler , NULL);
    pcap_freecode(&fp);
    pcap_close(handle);

    return 0;
}