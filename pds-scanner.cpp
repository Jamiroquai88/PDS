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

/*
 * @brief Function to handle CTRL+C for exiting.
 */
void signal_callback_handler(int signum)
{
	exit(-1);
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

    Interface inface = Interface(interface_name);

    return 0;
}
