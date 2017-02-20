/*
 * Interface.h
 *
 *  Created on: Feb 18, 2017
 *      Author: jose
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <vector>

#include "host.h"

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

		static int CompareUSChar(unsigned char * a, unsigned char * b, unsigned int size);
		/**
		 * Class members.
		 */
		int m_sockfd;
		int m_index;
		std::string m_name;
		char m_ip[16];
		unsigned char m_mac[6];

		std::vector<Host*> m_hosts;
};

#endif /* INTERFACE_H_ */
