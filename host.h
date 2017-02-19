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
	Host(std::string ipv4, std::string mac);
	virtual ~Host();

	std::string m_ipv4;
	std::string m_mac;
};

#endif /* HOST_H_ */
