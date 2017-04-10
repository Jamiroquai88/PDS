/*
 * spoofer.h
 *
 *  Created on: Apr 9, 2017
 *      Author: darthvader
 */

#ifndef SPOOFER_H_
#define SPOOFER_H_

#define ARP_PACKET_LEN	42

#include <string>

#include "host.h"
#include "protheader.h"


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

		bool Start();
		void Free();
		/**
		 * Class members.
		 */
		enum protocol_type { ARP, NDP };

	private:
		void FillARPHeader();
		void FillEthernetHeader();
		int OpenSocket();
		void ARPInjection(int sockfd);

		protocol_type m_protocol;
		Host::ip_address_type m_ipType;
		std::string m_interface;
		unsigned int m_interval;
		Host *mp_host1;
		Host *mp_host2;
		arp_header m_arpHeader;
		eth_header m_ethHeader;
};

#endif /* SPOOFER_H_ */
