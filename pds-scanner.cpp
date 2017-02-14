#include <iostream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <netinet/in.h>

#include "errormsg.h"

/**
 * @brief Main function for pds-scanner.
 */
int main(int argc, char * argv[] ) {
    std::string interface("");
    std::string out_xml("");
    int c;
    while ((c = getopt (argc, argv, "i:f:")) != -1) {
        switch (c) {
            case 'i':
                interface = optarg;
                break;
            case 'f':
                out_xml = optarg;
                break;
        }
    }
    if (interface.length() == 0 || out_xml.length() == 0) {
        print_abort("Invalid parameters!");
    }
        struct ifreq ifr;
        struct ifconf ifc;
        char buf[1024];
        int success = 0;

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock == -1) { /* handle error*/ };

        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { /* handle error */ }

        struct ifreq* it = ifc.ifc_req;
        const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

        for (; it != end; ++it) {
            strcpy(ifr.ifr_name, it->ifr_name);
            if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
                if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                        success = 1;
                        break;
                    }
                }
            }
            else { /* handle error */ }
        }

        unsigned char mac_address[6];

        if (success) memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
        std::cout << "lala" << mac_address[0];

    return 0;
}
