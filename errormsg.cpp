/*
 * errormsg.cpp
 *
 *  Created on: Apr 9, 2017
 *      Author: Jan Profant
 */

#include "errormsg.h"

/**
 * @brief Print help for scanner.
 */
void print_help_scanner() {
    std::cerr << "Usage: ./pds-scanner -i interface -f file" << std::endl;
}

/**
 * @brief Print help for chooser.
 */
void print_help_chooser() {
    std::cerr << "Usage: ./pds-chooser -f input_xml_file -o output_xml_file" << std::endl;
}

/**
 * @brief Print help for spoofer.
 */
void print_help_spoof() {
    std::cerr << "Usage: ./pds-spoof -i interface -t sec -p protocol -victim1ip ipaddress "
    		"-victim1mac macaddress -victim2ip ipaddress -victim2mac macaddress" << std::endl;
}

/**
 * @brief Print help for massspoofer.
 */
void print_help_massspoof() {
    std::cerr << "Usage: ./pds-massspoof -i interface -t sec -p protocol -f xml_file" << std::endl;
}

/**
 * @brief Print help for intercept.
 */
void print_help_intercept() {
	std::cerr << "Usage: ./pds-intercept -i interface -f xml_file" << std::endl;
}

/**
 * @brief Print error message and exit with error code.
 */
void print_msg_and_abort(std::string msg) {
    std::cerr << "ERROR: " << msg << std::endl;
    exit(-1);
}


