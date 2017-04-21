/*
 * host.h
 *
 *  Created on: Feb 19, 2017
 *      Author: Jan Profant
 */

#ifndef HOST_H_
#define HOST_H_

#include <string>
#include <vector>

/**
 * @brief Handles host as independent structure.
 */
class Host {
	public:
		Host(unsigned char *mac);
		Host();
		virtual ~Host();

		const unsigned char* 				GetMAC() const			{ return m_mac;  		}
		const std::vector<unsigned char *>  GetIPv4() const			{ return m_ipv4; 		}
		void AddIPv4(const unsigned char *ip);
		void AddIPv6(const unsigned int *ip);
		void SetMAC(unsigned char *mac);
		unsigned char* GetIPv4(unsigned int i) const				{ return m_ipv4[i]; 	}
		unsigned int* GetIPv6(unsigned int i) const					{ return m_ipv6[i];		}
		bool ExistsIPv4(const unsigned char *ip);
		bool ExistsIPv6(const unsigned int *ip);
		void Free();

		static int CompareUSChar(const unsigned char* a, const unsigned char* b, unsigned int size);
		static int CompareUSInt(const unsigned int* a, const unsigned int* b, unsigned int size);
		static void String2MAC(const char* src, unsigned char* dst);
		static void String2IPv4(const char* src, unsigned char* dst);
		static unsigned int String2IPv4(const char *ip);
		static void String2IPv6(const char *src, unsigned char *dst);
		static int IsValidIP(std::string ip);
		static bool IsValidMAC(std::string mac);
		static void PrintMAC(const unsigned char *mac);
		static void PrintIPv4(const unsigned char *ip);

		enum ip_address_type { IPv4, IPv6, INVALID };

		bool m_isUsed;

	private:
		std::vector<unsigned char *> m_ipv4;
		std::vector<unsigned int *> m_ipv6;
		unsigned char m_mac[6];
};

#endif /* HOST_H_ */
