/*
 * host.cpp
 *
 *  Created on: Feb 19, 2017
 *      Author: jose
 */

#define DEBUG

#include <iostream>
#include "host.h"
#include <stdio.h>
#include <string.h>



Host::Host(unsigned char *ipv4, unsigned char *mac) {
	memcpy(m_ipv4, ipv4, sizeof(unsigned char) * 4);
	memcpy(m_mac, mac, sizeof(unsigned char) * 6);
	#ifdef DEBUG
		printf("Host constructor IP: %u.%u.%u.%u, MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                                m_ipv4[0], m_ipv4[1],
                                m_ipv4[2], m_ipv4[3],
                                m_mac[0], m_mac[1],
                                m_mac[2], m_mac[3],
                                m_mac[4], m_mac[5]);
	#endif
}

Host::~Host() {
}

