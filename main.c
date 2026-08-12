#include <stdio.h>
#include <pcap.h>
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
    printf("Successfully opened device for capture");
    pcap_close(handle);
    return 0;
}