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
std::vector<pthread_t *> threads;

/*
 * @brief Function to handle CTRL+C for exiting.
 */
void signal_callback_handler(int signum)
{
	p_intercepter->Free();
	for (auto &i : threads) {
		delete i;
	}
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

	signal(SIGINT, signal_callback_handler);

	p_intercepter = new Intercepter();
	p_intercepter->SetInterface(interface_name);
	p_intercepter->SetFile(xml_file);

	pthread_t *pt1, *pt2;
	for (auto &i : p_intercepter->GetHostsMap()) {
		pt1 = new pthread_t();
		pthread_create(pt1, NULL, &Intercepter::Start_helper, p_intercepter);
		threads.push_back(pt1);
		pt2 = new pthread_t();
		pthread_create(pt2, NULL, &Intercepter::Start_helper, p_intercepter);
		threads.push_back(pt2);
	}
	pthread_join(*pt2, 0);

	return 0;
}
