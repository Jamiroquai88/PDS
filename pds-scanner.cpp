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
#include <sys/socket.h>

#include "errormsg.h"
#include "interface.h"

/**
 * Global variable for memory cleaning.
 */
Interface *inface;

/*
 * @brief Function to handle CTRL+C for exiting.
 */
void signal_callback_handler(int signum)
{
	inface->Free();
	exit(0);
}

/**
 * @brief Main function for pds-scanner.
 */
int main(int argc, char * argv[] ) {
    std::string interface_name("");
    std::string out_xml("");
    int c;
    while ((c = getopt (argc, argv, "i:f:")) != -1) {
        switch (c) {
            case 'i':
            	interface_name = optarg;
                break;
            case 'f':
                out_xml = optarg;
                break;
        }
    }
    if (interface_name.length() == 0 || out_xml.length() == 0) {
    	print_help_scanner();
    	print_msg_and_abort("Invalid parameters!");
    }

    if (getuid() && geteuid())
    	print_msg_and_abort("You must be root to run this.");

    signal(SIGINT, signal_callback_handler);

    inface = new Interface(interface_name);

    int sockfd;
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    	print_msg_and_abort("socket() failed\n ");
    inface->m_sockfd = sockfd;

    pthread_t pt1, pt2;
    pthread_create(&pt1, NULL, &Interface::Sniff_helper, &inface);
    pthread_create(&pt2, NULL, &Interface::Generate_helper, &inface);

    pthread_join(pt2, NULL);

    close(sockfd);

    return 0;
}
