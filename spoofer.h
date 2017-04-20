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
#include "interface.h"


class Spoofer {
	public:
		Spoofer();
		virtual ~Spoofer();

		void SetProtocolType(std::string t);
		bool SetInterface(std::string inface);
		void SetInterval(unsigned int time)                 { m_interval = time; 	}
		bool SetVictim1IP(std::string ip);
		bool SetVictim1MAC(std::string mac);
		bool SetVictim2IP(std::string ip);
		bool SetVictim2MAC(std::string mac);

		static void *StartVictim1_helper(void *context)     { return ((Spoofer *)context)->StartVictim1(); }
		static void *StartVictim2_helper(void *context)     { return ((Spoofer *)context)->StartVictim2(); }
		void *StartVictim1(void);
		void *StartVictim2(void);
		void Free();
		
		enum protocol_type { ARP, NDP };

	protected:
		/**
		 * Class members.
		 */
		protocol_type m_protocol;
		Host::ip_address_type m_ipType;
		std::string m_interface;
		unsigned int m_interval;
		Host *mp_host1;
		Host *mp_host2;
		Interface *mp_interface;
		arp_packet m_arpPacket1;
		arp_packet m_arpPacket2;

	private:
		void FillHeaders(arp_packet *h, unsigned int index);
		int OpenSocket();
		void ARPInjection(int sockfd, arp_packet *h);

};

#endif /* SPOOFER_H_ */
