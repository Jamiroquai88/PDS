/*
 * Interface.cpp
 *
 *  Created on: Feb 18, 2017
 *      Author: jose
 */

#include "interface.h"
#include "errormsg.h"

/**
 * Class constructor. Iterates through interfaces by name.
 * Based on: http://divingintolinux.sanupdas.com/?p=239
 */
Interface::Interface(std::string name) : m_sockfd(0), m_index(0) {
	struct ifaddrs *ifa = NULL;
	struct sockaddr_in *sa_in = NULL;
	struct ifreq ifr;

	if ((m_sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(0x0806))) < 0)
	    	print_msg_and_abort("socket() failed");

	if (getifaddrs(&ifa) < 0) {
		print_msg_and_abort("getifaddrs failed");
	}

	while (ifa != NULL) {
		if (ifa->ifa_addr->sa_family == AF_PACKET) {
			strcpy(ifr.ifr_name, ifa->ifa_name);
			if (ioctl(m_sockfd, SIOCGIFFLAGS, &ifr) && !ifr.ifr_flags & (IFF_UP|IFF_RUNNING))
				continue;
			if (name == ifa->ifa_name) {
				m_name = ifa->ifa_name;
				std::cout << "Interface name: " << m_name << std::endl;

				sa_in = (struct sockaddr_in *) ifa->ifa_addr;
				m_ip = inet_ntoa(sa_in->sin_addr);
				std::cout << "Interface ip: " << m_ip << std::endl;

				if (ioctl(m_sockfd, SIOCGIFHWADDR, &ifr) < 0) {
					freeifaddrs(ifa);
					print_msg_and_abort("ioctl SIOCGIFHWADDR failed");
				}
				m_mac = ifr.ifr_hwaddr.sa_data;
				std::cout << "Interface mac: " << ifr.ifr_hwaddr.sa_data << std::endl;

				if (ioctl(m_sockfd, SIOCGIFINDEX, &ifr) < 0) {
					freeifaddrs(ifa);
					print_msg_and_abort("ioctl SIOCGIFINDEX failed\n");
				}
				m_index = ifr.ifr_ifindex;
				std::cout << "Interface index: " << m_index << std::endl;
				break;
			}
		}
		ifa = ifa->ifa_next;
		close(m_sockfd);
	}
}

Interface::~Interface() {
	// TODO Auto-generated destructor stub
}

