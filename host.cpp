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

Host::Host(std::string ipv4, std::string mac) {
	std::cout << "Constructor " << ipv4 << " " << mac << std::endl;
	m_ipv4 = ipv4;
	m_mac = mac;
	std::cout << "Constructor2 " << m_ipv4 << " " << m_mac << std::endl;
}

Host::~Host() {
	// TODO Auto-generated destructor stub
}

