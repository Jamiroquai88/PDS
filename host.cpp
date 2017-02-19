/*
 * host.cpp
 *
 *  Created on: Feb 19, 2017
 *      Author: jose
 */

#include "host.h"
#include <stdio.h>
#include <string.h>

Host::Host(char ipv4[16], char mac[20]) {
	memcpy(m_ipv4, ipv4, 16);
	memcpy(m_mac, mac, 20);
}

Host::~Host() {
	// TODO Auto-generated destructor stub
}

