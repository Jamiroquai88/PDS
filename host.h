/*
 * host.h
 *
 *  Created on: Feb 19, 2017
 *      Author: jose
 */

#ifndef HOST_H_
#define HOST_H_

class Host {
public:
	Host(char ipv4[16], char mac[20]);
	virtual ~Host();

	char m_ipv4[16];
	char m_mac[20];
};

#endif /* HOST_H_ */
