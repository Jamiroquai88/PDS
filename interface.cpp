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

	if((m_sockfd = socket(AF_INET, SOCK_RAW, htons(IPPROTO_RAW))) < 0)
	    	print_msg_and_abort("socket() failed");

	if(getifaddrs(&ifa) < 0) {
		print_msg_and_abort("getifaddrs failed");
	}

	while(ifa != NULL) {
		if (ifa->ifa_addr->sa_family==AF_INET) {
			strcpy(ifr.ifr_name, ifa->ifa_name);
			if (ioctl(m_sockfd, SIOCGIFFLAGS, &ifr) && (!ifr.ifr_flags & (IFF_UP|IFF_RUNNING)))
				continue;
			std::cout << ifa->ifa_name;
			if (name ==  ifa->ifa_name) {
				m_name = ifa->ifa_name;

				sa_in = (struct sockaddr_in *) ifa->ifa_addr;
				m_ip = inet_ntoa(sa_in->sin_addr);

				if (ioctl(m_sockfd, SIOCGIFHWADDR, &ifr) < 0) {
					freeifaddrs(ifa);
					print_msg_and_abort("ioctl SIOCGIFHWADDR failed");
				}
				m_mac = ifr.ifr_hwaddr.sa_data;

				if (ioctl(m_sockfd, SIOCGIFINDEX, &ifr) < 0) {
					freeifaddrs(ifa);
					print_msg_and_abort("ioctl SIOCGIFINDEX failed\n");
				}
				m_index = ifr.ifr_ifindex;
				break;
			} else {
				continue;
			}
		}
	}
	freeifaddrs(ifa);
}

Interface::~Interface() {
	// TODO Auto-generated destructor stub
}

