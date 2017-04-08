/*
 * host.h
 *
 *  Created on: Feb 19, 2017
 *      Author: jose
 */

#ifndef HOST_H_
#define HOST_H_

#include <string>

class Host {
public:
	Host(std::vector<unsigned char> ipv4, std::vector<unsigned char> ipv6, unsigned char *mac);
	virtual ~Host();

	std::vector<unsigned char [4]> m_ipv4;
	std::vector<unsigned int [8]> m_ipv6[8];
	unsigned char m_mac[6];
};

#endif /* HOST_H_ */
