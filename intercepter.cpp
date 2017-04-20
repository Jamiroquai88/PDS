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

Intercepter::Intercepter() : mp_interface(0) {
}

Intercepter::~Intercepter() {
}

void Intercepter::SetInterface(std::string inface) {
	mp_interface = new Interface(inface);
}

void Intercepter::SetFile(std::string f) {
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL, node = NULL, dnode = NULL;/* node pointers */
	std::string ipv4, ipv6, mac;
	std::string strindex("");
	int index = 0;
	bool is_ipv4 = false;
	std::string node_content;

	if ((doc = xmlReadFile(f.c_str(), NULL, 0)) == NULL)
		print_msg_and_abort("Failed to open input XML file!");
	root_node = xmlDocGetRootElement(doc);
	for (node = root_node->children; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE && strcmp((char *)node->name, "host") == 0) {
			strindex = (const char*)xmlGetProp(node, (const xmlChar*)"group");
			mac = (const char*)xmlGetProp(node, (const xmlChar*)"mac");
			strindex = std::regex_replace(strindex, std::regex("victim-pair-"), "");
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

void Intercepter::Free() {
	mp_interface->Free();
	delete mp_interface;
}

void Intercepter::AddHost(unsigned int index, std::string mac) {
	unsigned char *m = new unsigned char[6];
	Host *p_host = new Host();
	m_hostsMap[index].push_back(p_host);
	Host::String2MAC(mac.c_str(), m);
	p_host->SetMAC(m);
}

void Intercepter::Start() {
	int sock_fd = OpenSocket();
	struct sockaddr_ll sll;

}

int Intercepter::OpenSocket() {
	int socket_fd = 0;
	struct timeval tv_limit;
	tv_limit.tv_sec = 2;
	tv_limit.tv_usec = 2000;

	if ((socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
		print_msg_and_abort("socket failed!");
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_limit, sizeof(struct timeval)) != 0)
		print_msg_and_abort("setsockopt failed!");
	if (setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, mp_interface->m_name.c_str(), sizeof(char) * mp_interface->m_name.length()))
		print_msg_and_abort("setsockopt failed!");

	return socket_fd;
}

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

void Intercepter::InitSSL(struct sockaddr_ll *sll) {
	memset(sll, 0, sizeof(struct sockaddr_ll));
	sll->sll_family = htons(AF_PACKET);
	sll->sll_protocol = htons(ETH_P_ALL);
	sll->sll_ifindex = if_nametoindex(mp_interface->m_name.c_str());
	sll->sll_halen = ETHER_ADDR_LEN;
	memcpy(sll->sll_addr, mp_interface->m_mac, ETHER_ADDR_LEN);
}
