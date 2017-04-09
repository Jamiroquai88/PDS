/*
 * spoofer.cpp
 *
 *  Created on: Apr 9, 2017
 *      Author: darthvader
 */

#include "spoofer.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdio>
#include <cstring>
#include <iostream>

Spoofer::Spoofer() : m_protocol(ARP), m_ipType(INVALID), m_interval(0) {
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
	int ip_type = IsValidIP(ip);
	if (ip_type == IPv4) {
		m_ipType = IPv4;
//		m_v1ip = ip;
		return true;
	}
	if (ip_type == IPv6) {
		m_ipType = IPv6;
//		m_v1ip = ip;
		return true;
	}
	return false;
}

bool Spoofer::SetVictim2IP(std::string ip) {
	int ip_type = IsValidIP(ip);
	if (ip_type == IPv4) {
		m_ipType = IPv4;
//		m_v2ip = ip;
		return true;
	}
	if (ip_type == IPv6) {
		m_ipType = IPv6;
//		m_v2ip = ip;
		return true;
	}
	return false;
}

int Spoofer::IsValidIP(std::string ip) {
	struct sockaddr_in sa;
	struct sockaddr_in6 sav6;
	int isv4 = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
	int isv6 = inet_pton(AF_INET6, ip.c_str(), &(sav6.sin6_addr));
	if (isv4 != -1)
		return IPv4;
	if (isv6 != -1)
		return IPv6;
	return INVALID;
}

void Spoofer::Start() {
}

/**
 * @brief Validate MAC address.
 * Based on: https://github.com/VincentDary/ArpSpoof/blob/master/CmdLineTtmt.c
 */
bool Spoofer::IsValidMAC(std::string mac) {
	const char *strMac = mac.c_str();
	int i = 0;
	if(strlen(strMac) != 17)
		return false;

	for (i=0; i < 17; ++i)
	{
		if(i==2)
		{
			if(strMac[i] != ':')
				return false;
		}
		else if(i==5)
		{
			if(strMac[i] != ':')
				return false;
		}
		else if(i==8)
		{
			if(strMac[i] != ':')
				return false;
		}
		else if(i==11)
		{
			if(strMac[i] != ':')
				return false;
		}
		else if(i==14)
		{
			if(strMac[i] != ':')
				return false;
		}
		else
		{
			if(strMac[i] < 0x30 || strMac[i] > 0x39)
				if(strMac[i] < 0x61 || strMac[i] > 0x66)
					if(strMac[i] < 0x41 || strMac[i] > 0x46)
						return false;
		}
	}
	return true;
}

bool Spoofer::SetVictim1MAC(std::string mac) {
	if (!IsValidMAC(mac))
		return false;
	else {
		unsigned char *p_mac = new unsigned char[6];
		p_mac = (unsigned char *)mac.c_str();
		sprintf((char *)p_mac, "%02x:%02x:%02x:%02x:%02x:%02x",
				p_mac[0], p_mac[1], p_mac[2],
				p_mac[3], p_mac[4], p_mac[5]);
		std::cout << p_mac;
		delete[] p_mac;
	}
	return true;
}

bool Spoofer::SetVictim2MAC(std::string mac) {
	if (!IsValidMAC(mac))
		return false;
	else {
		unsigned char *p_mac = new unsigned char[6];
		p_mac = (unsigned char *)mac.c_str();
		sprintf((char *)p_mac, "%02x:%02x:%02x:%02x:%02x:%02x",
				p_mac[0], p_mac[1], p_mac[2],
				p_mac[3], p_mac[4], p_mac[5]);
		std::cout << p_mac;
		delete[] p_mac;
		return true;
	}
}

void Spoofer::FillARPHeader() {
	m_arpHeader.hrd = htons(0x01);
	m_arpHeader.proto = htons(0x800);
	m_arpHeader.hrd_add_len = 0x06;
	m_arpHeader.proto_add_len = 0x04;
	m_arpHeader.op = htons(0x01);
//	m_arpHeader.smac =
}
