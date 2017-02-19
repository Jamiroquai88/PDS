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
	struct arp_header *arp_header;                             //build up the arp packet
	char eth_frame[ETH_FRAME_LEN];				// ethernet packet
	struct ethhdr *eth_header;				//build up ethernet header, from if_ether.h
	unsigned char dmac[ETH_ALEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};    //Ethernet dest. Address
	struct sockaddr_ll device;

	device.sll_ifindex = m_index;
	device.sll_family = AF_PACKET;
	memcpy(device.sll_addr, m_mac.c_str(), 6 * sizeof (uint8_t));
	device.sll_halen = htons(6);

	eth_header = (struct ethhdr*)eth_frame;                 //build up the ethernet packet
	memcpy(eth_header->h_dest, dmac, ETH_ALEN);
	eth_header->h_proto=htons(0x0806);			//0x0806 for Address Resolution Packet
	memcpy(eth_header->h_source, m_mac.c_str(),6);

	arp_header = (struct arp_header *)(eth_frame + ETH_HLEN);  //start address in mem
	arp_header->hrd = htons(0x0001);                        //0x0001 for 802.3 Frames
	arp_header->proto = htons (0x0800);
	arp_header->hrd_add_len = ETH_ALEN;                     // 6 for eth-mac addr
	arp_header->proto_add_len = 4;                          //4 for IPv4 addr
	arp_header->op = htons(0x0001);				//0x0001 for ARP Request
	inet_pton(AF_INET, m_ip.c_str(), arp_header->sip);
	memcpy(arp_header->smac, m_mac.c_str(), 6);
	memcpy(arp_header->dmac,dmac,ETH_ALEN);			//Set destination mac in arp-header
	bzero(arp_header->pad,18);				//Zero fill the packet until 64 bytes reached

	char *a, *b, *c, *d;
	char *tnet, *net, *toip;
	int i;

	net = (char *) malloc ((sizeof(char)) * 16);
	tnet = (char *) malloc ((sizeof(char)) * 16);
	toip = (char *) malloc ((sizeof(char)) * 16);

	sprintf(tnet, "%s", m_ip);
	a = strtok (tnet, "."); /* 1st ip octect */
	b = strtok (NULL, "."); /* 2nd ip octect */
	c = strtok (NULL, "."); /* 3rd ip octect */
	d = strtok (NULL, "."); /* 4th ip octect */

	sprintf(net, "%s.%s.%s", a, b, c);

	int n;
	//iterate over all arguments
	for(i = 1; i <= 255; i++) {
		sprintf(toip,"%s.%i", net, i);
		inet_pton (AF_INET, toip, arp_header->dip);
		if(n = sendto(m_sockfd, &eth_frame, 64, 0, (struct sockaddr *) &device, sizeof(device)) <= 0)
			print_msg_and_abort("failed to send\n");

		usleep(2 * 1000);
	}
	return 0;
}
