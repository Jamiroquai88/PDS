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
	memcpy(&m_ipv4, ipv4, sizeof(unsigned char) * 4);
	memcpy(&m_mac, mac, sizeof(unsigned char) * 6);
	std::cout << "Constructor2 " << m_ipv4 << " " << m_mac << std::endl;
}

Host::~Host() {
	// TODO Auto-generated destructor stub
}

