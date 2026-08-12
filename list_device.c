#include <stdio.h>
#include <pcap.h>

int main(){
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    pcap_if_t *d;

    if (pcap_findalldevs(&alldevs, &errbuf[0]) == -1){
        fprintf(stderr, "Error finding devices %s", errbuf);
        return 1;
    }

    int i = 0;
    for(d = alldevs; d != NULL; d = d->next){
        printf("%d , %s", ++i, d->name);
        if(d->description){
            printf(" (%s)", d->description);
        }
        printf("\n");
    }

    if(i == 0){
        printf("No interfaces found — are you running as Administrator?\n");
    }

    pcap_freealldevs(alldevs);

    return 0;
}