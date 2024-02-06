#ifdef _MSC_VER
/*
 * we do not want the warnings about the old deprecated and unsecure CRT functions
 * since these examples can be compiled under *nix as well
 */
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <pcap.h>
#include <cstdio>
#include <ctime>

#ifdef _WIN32

#include <tchar.h>

bool LoadNpcapDlls() {
    _TCHAR npcap_dir[512];
    UINT len;
    len = GetSystemDirectory(npcap_dir, 480);
    if (!len) {
        fprintf(stderr, "Error in GetSystemDirectory: %lx", GetLastError());
        return FALSE;
    }
    _tcscat_s(npcap_dir, 512, _T("\\Npcap"));
    if (SetDllDirectory(npcap_dir) == 0) {
        fprintf(stderr, "Error in SetDllDirectory: %lx", GetLastError());
        return FALSE;
    }
    return TRUE;
}

#endif

/* prototype of the packet handler */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

int main() {
#ifdef _WIN32
    if (!LoadNpcapDlls()) {
        fprintf(stderr, "Couldn't load Npcap\n");
        exit(1);
    }
#endif

    pcap_if_t *all_devs;
    char err_buf[PCAP_ERRBUF_SIZE];

    /* Retrieve the device list */
    if (pcap_findalldevs(&all_devs, err_buf) == -1) {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", err_buf);
        exit(1);
    }

    /* Print the list */
    int i = 0;
    pcap_if_t *d;
    for (d = all_devs; d; d = d->next) {
        printf("%d. %s", ++i, d->name);
        if (d->description)
            printf(" (%s)\n", d->description);
        else
            printf(" (No description available)\n");
    }
    if (i == 0) {
        printf("\nNo interfaces found! Make sure Npcap is installed.\n");
        return -1;
    }

    /* Input the number of interface. */
    printf("Enter the interface number (1-%d):", i);
    int i_num;
    scanf("%d", &i_num);

    if (i_num < 1 || i_num > i) {
        printf("\nInterface number out of range.\n");
        /* Free the device list */
        pcap_freealldevs(all_devs);
        return -1;
    }

    /* Jump to the selected adapter */
    for (d = all_devs, i = 0; i < i_num - 1; d = d->next, i++);

    /* Open the device/adaptor */
    pcap_t *ad_handle;
    if ((ad_handle = pcap_open_live(d->name,    // name of the device
                                   65536,            // portion of the packet to capture.
            // 65536 grants that the whole packet will be captured on all the MACs.
                                   1,                // promiscuous mode (nonzero means promiscuous)
                                   1000,            // read timeout
                                   err_buf            // error buffer
    )) == nullptr) {
        fprintf(stderr, "\nUnable to open the adapter. %s is not supported by Npcap\n", d->name);
        /* Free the device list */
        pcap_freealldevs(all_devs);
        return -1;
    }

    printf("\nlistening on %s...\n", d->description);

    /* At this point, we don't need any more the device list. Free it */
    pcap_freealldevs(all_devs);

    /* start the capture */
    pcap_loop(ad_handle, 0, packet_handler, nullptr);

    pcap_close(ad_handle);
    return 0;
}


/* Callback function invoked by libpcap for every incoming packet */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) {
    struct tm *l_time;
    char time_str[16];
    time_t local_tv_sec;

    /*
     * unused parameters
     */
    (VOID) (param);
    (VOID) (pkt_data);

    /* convert the timestamp to readable format */
    local_tv_sec = header->ts.tv_sec;
    l_time = localtime(&local_tv_sec);
    strftime(time_str, sizeof time_str, "%H:%M:%S", l_time);

    printf("%s,%.6d len:%d\n", time_str, header->ts.tv_usec, header->len);

}
