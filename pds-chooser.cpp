/*
 * pds-schooser.cpp
 *
 *  Created on: Apr 8, 2017
 *      Author: Jan Profant
 */
#include <iostream>
#include <string>

#include "errormsg.h"
#include "pdsxmlparser.h"

/**
 * @brief Main function for pds-chooser.
 */
int main(int argc, char * argv[] ) {
	std::string in_xml("");
	std::string out_xml("");
	int c;
	while ((c = getopt (argc, argv, "f:o:")) != -1) {
		switch (c) {
			case 'f':
				in_xml = optarg;
				break;
			case 'o':
				out_xml = optarg;
				break;
		}
	}

	if (in_xml.length() == 0 || out_xml.length() == 0) {
		print_help_chooser();
		print_msg_and_abort("Invalid parameters!");
	}
	PDSXMLParser::ChooseVictimPairs(in_xml, out_xml);
	return 0;
}





