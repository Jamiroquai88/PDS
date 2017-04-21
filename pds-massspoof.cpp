/*
 * pds-massspoof.cpp
 *
 *  Created on: Apr 20, 2017
 *      Author: Jan Profant
 */

#include <getopt.h>
#include <cstring>
#include <iostream>
#include <string>

#include "errormsg.h"
#include "massspoofer.h"

/**
 * Global variable for memory cleaning.
 */
MassSpoofer *p_spoofer = 0;

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
 * @brief Main function for pds-massspoof.
 */
int main(int argc, char * argv[] ) {
	int c;
	std::string interface_name(""), protocol(""), xml_file("");
	int time = 0;
	while ((c = getopt(argc, argv, "i:t:p:f:")) != -1) {
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
			case 'f':
				xml_file = optarg;
				break;
		}
	}

	if (interface_name.length() == 0 || protocol.length() == 0 || time <= 0 || xml_file.length() == 0) {
		print_help_massspoof();
		print_msg_and_abort("Invalid parameters!");
	}

	if (getuid() && geteuid())
		print_msg_and_abort("You must be root to run this.");

	signal(SIGINT, signal_callback_handler);

	p_spoofer = new MassSpoofer();
	p_spoofer->SetInterval(time);
	p_spoofer->SetProtocolType(protocol);
	p_spoofer->SetInterface(interface_name);
	if (!p_spoofer->SetFile(xml_file))
		print_msg_and_abort("Can not parse input XML file!");
	p_spoofer->Start();

	return 0;
}
