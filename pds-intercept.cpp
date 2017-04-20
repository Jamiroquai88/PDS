/*
 * pds-intercept.cpp
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
#include "intercepter.h"

/**
 * Global variable for memory cleaning.
 */
Intercepter *p_intercepter = 0;

/*
 * @brief Function to handle CTRL+C for exiting.
 */
void signal_callback_handler(int signum)
{
	p_intercepter->Free();
	exit(0);
}

/**
 * @brief Main function for pds-intercept.
 */
int main(int argc, char * argv[] ) {
	int c;
	std::string interface_name(""), xml_file("");
	while ((c = getopt(argc, argv, "i:f:")) != -1) {
		switch (c) {
			case 'i':
				interface_name = optarg;
				break;
			case 'f':
				xml_file = optarg;
				break;
		}
	}

	if (interface_name.length() == 0 || xml_file.length() == 0) {
		print_help_intercept();
		print_msg_and_abort("Invalid parameters!");
	}

	if (getuid() && geteuid())
		print_msg_and_abort("You must be root to run this.");

	p_intercepter = new Intercepter();
	p_intercepter->SetInterface(interface_name);
	p_intercepter->SetFile(xml_file);
	p_intercepter->Start();

	return 0;
}
