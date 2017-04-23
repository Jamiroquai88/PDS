/*
 * intercepter.cpp
 *
 *  Created on: Apr 20, 2017
 *      Author: Jan Profant
 */

#include "intercepter.h"
#include "errormsg.h"
#include "host.h"

#include <iostream>
#include <regex>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <net/ethernet.h>
#include <iomanip>

/**
 * @brief Class constructor.
 */
Intercepter::Intercepter() : mp_interface(0) {
}

/**
 * @brief Class destructor.
 */
Intercepter::~Intercepter() {
}

/**
 * @bref Sets interface name.
 */
void Intercepter::SetInterface(std::string inface) {
	mp_interface = new Interface(inface);
}

/**
 * @brief Sets and loads input xml file.
 */
void Intercepter::SetFile(std::string f) {
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL, node = NULL, dnode = NULL;/* node pointers */
	std::string ipv4, ipv6, mac;
	std::string strindex("");
	int index = 0;
	bool is_ipv4 = false;
	std::string node_content;
	xmlChar *group_attr;
	size_t delim = 0;

	if ((doc = xmlReadFile(f.c_str(), NULL, 0)) == NULL)
		print_msg_and_abort("Failed to open input XML file!");
	root_node = xmlDocGetRootElement(doc);
	for (node = root_node->children; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE && strcmp((char *)node->name, "host") == 0) {
			group_attr = xmlGetProp(node, (const xmlChar*)"group");
			if (group_attr == 0)
				continue;
			strindex = (const char*)group_attr;
			mac = (const char*)xmlGetProp(node, (const xmlChar*)"mac");
			delim = strindex.find_last_of("-");
			strindex = strindex.substr(delim + 1, strindex.length() - delim - 1);
			index = atoi(strindex.c_str());
			AddHost(index, mac);
			for (dnode = node->children; dnode; dnode = dnode->next) {
				if (dnode->type == XML_ELEMENT_NODE) {
					node_content = (const char*)xmlNodeGetContent(dnode);
					if (strcmp((char *)dnode->name, "ipv4") == 0)
						is_ipv4 = true;
					if (strcmp((char *)dnode->name, "ipv6") == 0)
						is_ipv4 = false;
					AddAddress(index, node_content, is_ipv4);
				}
			}

		}
	}
}

/**
 * @brief Frees data structures.
 */
void Intercepter::Free() {
	mp_interface->Free();
	delete mp_interface;
}

/**
 * @brief Adds host as member.
 */
void Intercepter::AddHost(unsigned int index, std::string mac) {
	unsigned char *m = new unsigned char[6];
	Host *p_host = new Host();
	m_hostsMap[index].push_back(p_host);
	Host::String2MAC(mac.c_str(), m);
	p_host->SetMAC(m);
}

/**
 * @brief Runs interception.
 */
void *Intercepter::Start(void) {
	int sock_fd = OpenSocket();
	struct sockaddr_ll sll;
	InitSLL(&sll);
	unsigned char *buffer = new unsigned char[ETH_FRAME_SIZE];
	Host *host1 = 0, *host2 = 0;
	int received = 0;
	for (auto &i : m_hostsMap) {
		if (!i.second[0]->m_isUsed) {
			host1 = i.second[0];
			host1->m_isUsed = true;
			host2 = i.second[1];
			break;
		}
		else if (!i.second[1]->m_isUsed) {
			host1 = i.second[1];
			host1->m_isUsed = true;
			host2 = i.second[0];
			break;
		}
	}
	const unsigned char *src = host1->GetMAC();
	const unsigned char *dst = host2->GetMAC();
	while (true) {
		if ((received = recv(sock_fd, buffer, ETH_FRAME_SIZE, 0)) < 0)
			continue;
		if (Host::CompareUSChar(buffer + ETHER_ADDR_LEN, src, ETHER_ADDR_LEN) == 0) {
#ifdef DEBUG
			std::cout << "INFO: Detected source MAC address." << std::endl;
			std::cout << "      Source: ";
			Host::PrintMAC(src);
			std::cout << " ";
			Host::PrintIPv4(host1->GetIPv4(0));
			std::cout << ", destination: ";
			Host::PrintMAC(dst);
			std::cout << " ";
			Host::PrintIPv4(host2->GetIPv4(0));
			std::cout << ", sending from: ";
			Host::PrintMAC(mp_interface->m_mac);
			std::cout << std::endl;
			for(int i = 0; i < received; i++)
				printf("%02x ", buffer[i]);
			std::cout << std::endl;
#endif
			memcpy(buffer, dst, ETHER_ADDR_LEN);
			memcpy(buffer + ETHER_ADDR_LEN, mp_interface->m_mac, ETHER_ADDR_LEN);
			sendto(sock_fd, (const void *)buffer, received, 0,
					(const struct sockaddr *)&sll, sizeof(struct sockaddr_ll));
		}
	}
	return 0;
}

/**
 * @brief Opens socket.
 */
int Intercepter::OpenSocket() {
	int socket_fd = 0;
	struct timeval te;
	te.tv_sec = 0;
	te.tv_usec = 40000;

	if ((socket_fd = socket(AF_PACKET, SOCK_RAW, htons(0x0800))) == -1)
		print_msg_and_abort("socket failed!");
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&te, sizeof(struct timeval)) != 0)
		print_msg_and_abort("setsockopt failed!");
	if (setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, mp_interface->m_name.c_str(), sizeof(char) * mp_interface->m_name.length()))
		print_msg_and_abort("setsockopt failed!");

	return socket_fd;
}

/**
 * @brief Add IPv4/IPv6 address as member.
 */
void Intercepter::AddAddress(unsigned int index, std::string ip, bool ipv4) {
	unsigned char *ip_addr;
	std::map<unsigned int, std::vector<Host *>>::const_iterator pos = m_hostsMap.find(index);
	Host *p_host = pos->second.back();
	if (ipv4) {
		ip_addr = new unsigned char[4];
		Host::String2IPv4(ip.c_str(), ip_addr);
		p_host->AddIPv4(ip_addr);
	}
	else {
		ip_addr = new unsigned char[16];
		Host::String2IPv6(ip.c_str(), ip_addr);
		p_host->AddIPv6((unsigned int *)ip_addr);
	}
}

/**
 * @brief Initializes sockaddr_ll structure.
 */
void Intercepter::InitSLL(sockaddr_ll *sll) {
	memset(sll, 0, sizeof(struct sockaddr_ll));
	sll->sll_family = htons(AF_PACKET);
	sll->sll_protocol = htons(0x0800);
	sll->sll_ifindex = if_nametoindex(mp_interface->m_name.c_str());
	sll->sll_halen = ETHER_ADDR_LEN;
	memcpy(sll->sll_addr, mp_interface->m_mac, ETHER_ADDR_LEN);
}
