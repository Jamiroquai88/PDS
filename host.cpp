/*
 * host.cpp
 *
 *  Created on: Feb 19, 2017
 *      Author: jose
 */

#include <iostream>
#include "host.h"
#include <stdio.h>
#include <string.h>

Host::Host(unsigned char *ipv4, unsigned char *mac) {
	char buff_ipv4[16];
	sprintf(buff_ipv4, "%u.%u.%u.%u", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
	std::cout << buff_ipv4 << std::endl;
	std::cout << "Constructor2 " << ipv4 << " " << mac << std::endl;
	memcpy(&m_ipv4, ipv4, sizeof(unsigned char) * 4);
	memcpy(&m_mac, mac, sizeof(unsigned char) * 6);
	std::cout << "Constructor2 " << m_ipv4 << " " << m_mac << std::endl;
}

Host::~Host() {
	// TODO Auto-generated destructor stub
}

