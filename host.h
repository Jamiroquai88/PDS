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
	Host(unsigned char *ipv4, unsigned char *mac);
	virtual ~Host();

	unsigned char m_ipv4[4];
	unsigned char m_mac[6];
};

#endif /* HOST_H_ */
