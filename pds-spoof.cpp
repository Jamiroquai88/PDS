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

/**
 * Global variable for memory cleaning.
 */
Interface *inface = NULL;

// Long command line options
static struct option long_options[] = {
	{"victim1ip", required_argument, 0, 0},
	{"victim1mac", required_argument, 0, 0},
	{"victim2ip", required_argument, 0, 0},
	{"victim2mac", required_argument, 0, 0},
	{0,0,0,0}
};

/**
 * @brief Main function for pds-spoof.
 */
int main(int argc, char * argv[] ) {
	int c;
	std::string interface_name(""), protocol(""), v1ip(""), v1mac(""), v2ip(""), v2mac("");
	int time = 0;
	while (1) {
		int option_index = 0;
		c = getopt_long (argc, argv, "i:t:p:", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
			case 'i':
				interface_name = optarg;
				break;
			case 't':
				time = atoi(optarg);
				break;
			case 'p':
				protocol = optarg;
				break;
			case 0:
				if (long_options[option_index].flag != 0)
					break;
				if (strcmp(long_options[option_index].name, "victim1ip") == 0)
					v1ip = optarg;
				if (strcmp(long_options[option_index].name, "victim1mac") == 0)
					v1mac = optarg;
				if (strcmp(long_options[option_index].name, "victim2ip") == 0)
					v2ip = optarg;
				if (strcmp(long_options[option_index].name, "victim2mac") == 0)
					v2mac = optarg;
				break;
		}
	}
//	if (inet_pton(AF_INET, v1ip, ))
		
	if (interface_name.length() == 0 || protocol.length() == 0 || time <= 0) {
		print_help_scanner();
		print_msg_and_abort("Invalid parameters!");
	}
		
	inface = new Interface(interface_name);
	std::cout << inface << " " << protocol << " " << time << " " << v1ip << " " << v2ip
			<< " " << v1mac  << " " << v2mac  << " " << std::endl;
	return 0;
}


