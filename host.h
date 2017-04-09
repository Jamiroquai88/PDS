/*
 * host.h
 *
 *  Created on: Feb 19, 2017
 *      Author: jose
 */

#ifndef HOST_H_
#define HOST_H_

#include <string>
#include <vector>

class Host {
	public:
		Host(unsigned char *mac);
		virtual ~Host();

		const unsigned char* 				GetMAC() const			{ return m_mac;  }
		const std::vector<unsigned char *>  GetIPv4() const			{ return m_ipv4; }
		void AddIPv4(const unsigned char *ip);
		void AddIPv6(const unsigned int *ip);
		bool ExistsIPv4(const unsigned char *ip);
		bool ExistsIPv6(const unsigned int *ip);
		void Free();

		static int CompareUSChar(const unsigned char * a, const unsigned char * b, unsigned int size);
		static int CompareUSInt(const unsigned int * a, const unsigned int * b, unsigned int size);
	private:
		std::vector<unsigned char *> m_ipv4;
		std::vector<unsigned int *> m_ipv6;
		unsigned char m_mac[6];
};

#endif /* HOST_H_ */
