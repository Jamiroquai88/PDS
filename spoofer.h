/*
 * spoofer.h
 *
 *  Created on: Apr 9, 2017
 *      Author: darthvader
 */

#ifndef SPOOFER_H_
#define SPOOFER_H_

#include <string>

#include "arpheader.h"
#include "host.h"


class Spoofer {
	public:
		Spoofer();
		virtual ~Spoofer();

		void SetProtocolType(std::string t);
		void SetInterface(std::string inface)				{ m_interface = inface; }
		void SetInterval(unsigned int time)					{ m_interval = time; 	}
		bool SetVictim1IP(std::string ip);
		bool SetVictim1MAC(std::string mac);
		bool SetVictim2IP(std::string ip);
		bool SetVictim2MAC(std::string mac);

		void Start();
		/**
		 * Class members.
		 */
		enum protocol_type { ARP, NDP };
		enum ip_address_type { IPv4, IPv6, INVALID };

	private:
		static int IsValidIP(std::string ip);
		static bool IsValidMAC(std::string mac);
		void FillARPHeader();

		protocol_type m_protocol;
		ip_address_type m_ipType;
		std::string m_interface;
		unsigned int m_interval;
		Host *mp_host1;
		Host *mp_host2;
		arp_header m_arpHeader;
};

#endif /* SPOOFER_H_ */
