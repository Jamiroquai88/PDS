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

Host::Host(char ipv4[16], char mac[20]) {
	std::cout << "Constructor" << ipv4 << " " << mac << std::endl;
	strcpy(m_ipv4, ipv4);
	strcpy(m_mac, mac);
}

Host::~Host() {
	// TODO Auto-generated destructor stub
}

