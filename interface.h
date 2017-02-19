/*
 * Interface.h
 *
 *  Created on: Feb 18, 2017
 *      Author: jose
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

/**
 * Class Interface represent current active interface.
 */
class Interface {
	public:
		Interface(std::string name);
		virtual ~Interface();

		void *Sniff(void);
		void *Generate(void);

		static void *Sniff_helper(void *context) 							{ return ((Interface *)context)->Sniff(); }
		static void *Generate_helper(void *context) 						{ return ((Interface *)context)->Generate(); }

		/**
		 * Class members.
		 */
		int m_sockfd;
		int m_index;
		std::string m_name;
		std::string	m_ip;
		std::string m_mac;
};

#endif /* INTERFACE_H_ */
