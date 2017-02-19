/*
 * Interface.cpp
 *
 *  Created on: Feb 18, 2017
 *      Author: jose
 */

#include "interface.h"
#include "errormsg.h"
#include "arpheader.h"

/**
 * Class constructor. Iterates through interfaces by name.
 * Based on: http://divingintolinux.sanupdas.com/?p=239
 */
Interface::Interface(std::string name) : m_sockfd(0), m_index(0) {
	struct ifaddrs *ifa = NULL;
	struct sockaddr_in *sa_in = NULL;
	struct ifreq ifr;

	if ((m_sockfd = socket(AF_INET, SOCK_RAW, htons(IPPROTO_RAW))) < 0)
	    	print_msg_and_abort("socket() failed");

	if (getifaddrs(&ifa) < 0) {
		print_msg_and_abort("getifaddrs failed");
	}

	while (ifa != NULL) {
		if (ifa->ifa_addr->sa_family == AF_INET) {
			strcpy(ifr.ifr_name, ifa->ifa_name);
			if (ioctl(m_sockfd, SIOCGIFFLAGS, &ifr) && (!ifr.ifr_flags & (IFF_UP|IFF_RUNNING)))
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
	}
	close(m_sockfd);
}

Interface::~Interface() {

}

void *Interface::Sniff() {
	char buffer[65535];
	struct arp_header *arp_rply;
	char mac[20];
	struct sockaddr from;
	char vendor[20];

	int r = 0;

	arp_rply = (struct arp_header *)((struct packet*)(buffer+14));

		while(1) {
			r = recv(m_sockfd, buffer, sizeof(buffer), 0);
			if(((((buffer[12])<<8)+buffer[13])!=ETH_P_ARP) && ntohs(arp_rply->op)!=2)
				continue;

			printf("%u.%u.%u.%u\t",
					arp_rply->sip[0], arp_rply->sip[1],
					arp_rply->sip[2], arp_rply->sip[3]);

			sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
					arp_rply->smac[0], arp_rply->smac[1],
					arp_rply->smac[2], arp_rply->smac[3],
					arp_rply->smac[4], arp_rply->smac[5]);
			printf("%s\t", mac);

			sprintf(vendor,"%02x%02x%02x%02x%02x%02x",
					arp_rply->smac[0], arp_rply->smac[1],
					arp_rply->smac[2], arp_rply->smac[3],
					arp_rply->smac[4], arp_rply->smac[5]);

//			printf("%s\n", get_vendor(vendor));
		}
		close(m_sockfd);
		printf("calling exit\n");
		exit(0);
}

void *Interface::Generate() {
	while (1) {};
	return 0;
}

