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

	if ((m_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror(NULL);
		print_msg_and_abort("socket() failed");
	}

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
				memcpy(m_ip, inet_ntoa(sa_in->sin_addr), 16);
				std::cout << "Interface IP: " << m_ip << std::endl;

				if (ioctl(m_sockfd, SIOCGIFHWADDR, &ifr) < 0) {
					freeifaddrs(ifa);
					print_msg_and_abort("ioctl SIOCGIFHWADDR failed");
				}
				memcpy(m_mac, ifr.ifr_hwaddr.sa_data, 6);
				printf("Interface MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", m_mac[0], m_mac[1],
						m_mac[2], m_mac[3], m_mac[4], m_mac[5]);

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

/**
 * @brief Sniff for communication.
 * based on: http://divingintolinux.sanupdas.com/?p=239
 */
void *Interface::Sniff() {
	char buffer[65535];
	struct arp_header *arp_rply;
	char mac[20];
	char ipv4[16];

	arp_rply = (struct arp_header *)((struct packet*)(buffer+14));

	while(1) {
		recv(m_sockfd, buffer, sizeof(buffer), 0);
		if(((((buffer[12])<<8)+buffer[13])!=ETH_P_ARP) && ntohs(arp_rply->op)!=2)
			continue;

		for (auto &i : m_hosts) {
			sprintf(ipv4, "%u.%u.%u.%u", arp_rply->sip[0], arp_rply->sip[1],
					arp_rply->sip[2], arp_rply->sip[3]);
			sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
					arp_rply->smac[0], arp_rply->smac[1],
					arp_rply->smac[2], arp_rply->smac[3],
					arp_rply->smac[4], arp_rply->smac[5]);
			std::cout << "State: " << i->m_ipv4 << " " << i->m_mac << std::endl;
			if (strncmp(i->m_ipv4, ipv4, 16) && strncmp(i->m_mac, mac, 20))
				break;
		}
		std::cout << "Pushing " << ipv4 << " " << mac << " size " << m_hosts.size() << std::endl;
		m_hosts.push_back(new Host(ipv4, mac));

//		printf("%u.%u.%u.%u\t",
//				arp_rply->sip[0], arp_rply->sip[1],
//				arp_rply->sip[2], arp_rply->sip[3]);
//
//		sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
//				arp_rply->smac[0], arp_rply->smac[1],
//				arp_rply->smac[2], arp_rply->smac[3],
//				arp_rply->smac[4], arp_rply->smac[5]);

	}
	close(m_sockfd);
	exit(0);
}

/**
 * @brief Generate communication.
 * based on: http://divingintolinux.sanupdas.com/?p=239
 */
void *Interface::Generate() {
	struct arp_header *arp_header;                             //build up the arp packet
	char eth_frame[ETH_FRAME_LEN];				// ethernet packet
	struct ethhdr *eth_header;				//build up ethernet header, from if_ether.h
	unsigned char dmac[ETH_ALEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};    //Ethernet dest. Address
	struct sockaddr_ll device;

	device.sll_ifindex = m_index;
	device.sll_family = AF_PACKET;
	memcpy(device.sll_addr, m_mac, 6 * sizeof (uint8_t));
	device.sll_halen = htons(6);

	eth_header = (struct ethhdr*)eth_frame;                 //build up the ethernet packet
	memcpy(eth_header->h_dest, dmac, ETH_ALEN);
	eth_header->h_proto=htons(0x0806);			//0x0806 for Address Resolution Packet
	memcpy(eth_header->h_source, m_mac, 6);

	arp_header = (struct arp_header *)(eth_frame + ETH_HLEN);  //start address in mem
	arp_header->hrd = htons(0x0001);                        //0x0001 for 802.3 Frames
	arp_header->proto = htons (0x0800);
	arp_header->hrd_add_len = ETH_ALEN;                     // 6 for eth-mac addr
	arp_header->proto_add_len = 4;                          //4 for IPv4 addr
	arp_header->op = htons(0x0001);				//0x0001 for ARP Request
	inet_pton(AF_INET, m_ip, arp_header->sip);
	memcpy(arp_header->smac, m_mac, 6);
	memcpy(arp_header->dmac,dmac,ETH_ALEN);			//Set destination mac in arp-header
	bzero(arp_header->pad,18);				//Zero fill the packet until 64 bytes reached

	char *a, *b, *c;
	char *tnet, *net, *toip;
	int i;

	net = (char *) malloc ((sizeof(char)) * 16);
	tnet = (char *) malloc ((sizeof(char)) * 16);
	toip = (char *) malloc ((sizeof(char)) * 16);

	sprintf(tnet, "%s", m_ip);
	a = strtok (tnet, "."); /* 1st ip octect */
	b = strtok (NULL, "."); /* 2nd ip octect */
	c = strtok (NULL, "."); /* 3rd ip octect */

	sprintf(net, "%s.%s.%s", a, b, c);

	int n;
	while (1) {
		for(i = 1; i <= 255; i++) {
			sprintf(toip,"%s.%i", net, i);
			inet_pton(AF_INET, toip, arp_header->dip);
			if((n = sendto(m_sockfd, &eth_frame, 64, 0, (struct sockaddr *) &device, sizeof(device))) <= 0)
				print_msg_and_abort("failed to send\n");
		}
	}
	return 0;
}

