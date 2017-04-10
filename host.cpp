/*
 * host.cpp
 *
 *  Created on: Feb 19, 2017
 *      Author: Jan Profant
 */

#include "host.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstring>



Host::Host(unsigned char *mac) {
	memcpy(m_mac, mac, sizeof(unsigned char) * 6);
	#ifdef DEBUG
		printf("Host constructor MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                                m_mac[0], m_mac[1],
                                m_mac[2], m_mac[3],
                                m_mac[4], m_mac[5]);
	#endif
}

Host::Host() {
}

Host::~Host() {
}

void Host::AddIPv4(const unsigned char *ip) {
	unsigned char *p_ip = new unsigned char[4];
	memcpy(p_ip, ip, sizeof(unsigned char) * 4);
	m_ipv4.push_back(p_ip);
}

void Host::AddIPv6(const unsigned int *ip) {
	unsigned int *p_ip = new unsigned int[8];
	memcpy(p_ip, ip, sizeof(unsigned int) * 8);
	m_ipv6.push_back(p_ip);
}

bool Host::ExistsIPv4(const unsigned char* ip) {
	for (auto &x : m_ipv4) {
		if (CompareUSChar(x, ip, 4) == 0)
			return true;
	}
	return false;
}

bool Host::ExistsIPv6(const unsigned int* ip) {
	for (auto &x : m_ipv6) {
		if (CompareUSInt(x, ip, 8) == 0)
			return true;
	}
	return false;
}

void Host::Free() {
	for (auto &i : m_ipv4)
		delete[] i;
	for (auto &i : m_ipv6)
		delete[] i;
}

int Host::CompareUSChar(const unsigned char * a, const unsigned char * b, unsigned int size)
{
#ifdef DEBUG
	if (size == 4) {
		printf("Interface::CompareUSChar IPa: %u.%u.%u.%u\n", a[0], a[1], a[2], a[3]);
		printf("Interface::CompareUSChar IPb: %u.%u.%u.%u\n", b[0], b[1], b[2], b[3]);
	}
	if (size == 6) {
		printf("Interface::CompareUSChar MACa: %02x:%02x:%02x:%02x:%02x:%02x\n", a[0], a[1], a[2], a[3], a[4], a[5]);
		printf("Interface::CompareUSChar MACb: %02x:%02x:%02x:%02x:%02x:%02x\n", a[0], a[1], a[2], a[3], a[4], a[5]);
	}
#endif

	for (unsigned int i = 0; i < size; i++)
    		if (a[i] != b[i])
		return 1;
	return 0;
}

int Host::CompareUSInt(const unsigned int* a, const unsigned int* b, unsigned int size) {
	for (unsigned int i = 0; i < size; i++)
	    if (a[i] != b[i])
			return 1;
	return 0;
}

void Host::String2MAC(const char* src, unsigned char* dst) {
	sscanf(src, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			&dst[0], &dst[1], &dst[2], &dst[3], &dst[4], &dst[5]);
}

void Host::String2IPv4(const char* src, unsigned char* dst) {
	sscanf(src, "%hhu.%hhu.%hhu.%hhu",
			&dst[0], &dst[1], &dst[2], &dst[3]);
}

void Host::SetMAC(unsigned char* mac) {
	memcpy(m_mac, mac, sizeof(unsigned char) * 6);
}

/**
 * @brief Validate MAC address.
 * Based on: https://github.com/VincentDary/ArpSpoof/blob/master/CmdLineTtmt.c
 */
bool Host::IsValidMAC(std::string mac) {
	const char *strMac = mac.c_str();
	int i = 0;
	if(strlen(strMac) != 17)
		return false;
	for (i=0; i < 17; ++i)
	{
		if(i==2) {
			if(strMac[i] != ':')
				return false;
		}
		else if(i==5) {
			if(strMac[i] != ':')
				return false;
		}
		else if(i==8) {
			if(strMac[i] != ':')
				return false;
		}
		else if(i==11) {
			if(strMac[i] != ':')
				return false;
		}
		else if(i==14) {
			if(strMac[i] != ':')
				return false;
		}
		else {
			if(strMac[i] < 0x30 || strMac[i] > 0x39)
				if(strMac[i] < 0x61 || strMac[i] > 0x66)
					if(strMac[i] < 0x41 || strMac[i] > 0x46)
						return false;
		}
	}
	return true;
}

int Host::IsValidIP(std::string ip) {
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
