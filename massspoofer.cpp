/*
 * massspoofer.cpp
 *
 *  Created on: Apr 20, 2017
 *      Author: Jan Profant
 */

#include "massspoofer.h"
#include "spoofer.h"
#include "errormsg.h"

#include <iostream>
#include <regex>
#include <libxml/parser.h>
#include <libxml/tree.h>

/**
 * @brief Class constructor.
 */
MassSpoofer::MassSpoofer() : m_ipType(Host::INVALID), m_interval(0) {
}

/**
 * @brief Class destructor.
 */
MassSpoofer::~MassSpoofer() {
}

/**
 * @brief Start mass spoofing.
 */
void MassSpoofer::Start() {
	pthread_t *pt1, *pt2;
	for (unsigned int i = 0; i < m_spoofTuples.size(); i++) {
		pt1 = new pthread_t();
		pthread_create(pt1, NULL, &Spoofer::StartVictim1_helper, m_spoofTuples[i]);
		m_threads.push_back(pt1);
		pt2 = new pthread_t();
		pthread_create(pt2, NULL, &Spoofer::StartVictim2_helper, m_spoofTuples[i]);
		m_threads.push_back(pt2);
	}
	pthread_join(*pt2, 0);
}

/**
 * @brief Sets name of interface.
 */
void MassSpoofer::SetInterface(std::string inface) {
	 m_interface = inface;
}

/**
 * @brief Sets and loads input XML file.
 */
bool MassSpoofer::SetFile(std::string f) {
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL, node = NULL, dnode = NULL;/* node pointers */
	std::string ipv4, mac;
	std::string strindex("");
	int index = 0;
	std::string node_content;
	xmlChar *group_attr;

	if ((doc = xmlReadFile(f.c_str(), NULL, 0)) == NULL)
		return false;
	root_node = xmlDocGetRootElement(doc);
	for (node = root_node->children; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE && strcmp((char *)node->name, "host") == 0) {
			group_attr = xmlGetProp(node, (const xmlChar*)"group");
			if (group_attr == 0)
				continue;
			strindex = (const char*)group_attr;
			mac = (const char*)xmlGetProp(node, (const xmlChar*)"mac");
			strindex = std::regex_replace(strindex, std::regex("victim-pair-"), "");
			index = atoi(strindex.c_str());
			for (dnode = node->children; dnode; dnode = dnode->next) {
				if (dnode->type == XML_ELEMENT_NODE) {
					node_content = (const char*)xmlNodeGetContent(dnode);
					if (strcmp((char *)dnode->name, "ipv4") == 0)
						ipv4 = node_content;
				}
			}
			Add(index, ipv4, mac);
#ifdef DEBUG
			std::cout << "Index: " << index << " IPv4: " << ipv4 << " MAC: " << mac << std::endl;
#endif
		}
	}
	return true;
}

/**
 * @brief Frees class data structures.
 */
void MassSpoofer::Free() {
	for (auto &i : m_threads) {
		delete i;
	}
	for (auto &i : m_spoofTuples) {
		i->ResetARP();
		i->Free();
		delete i;
	}
}

/**
 * @brief Adds host and creates spoofing pair.
 */
void MassSpoofer::Add(unsigned int index, std::string ipv4, std::string mac) {
	if (m_spoofTuples.size() < index) {
		m_spoofTuples.push_back(new Spoofer());
		if (!m_spoofTuples[index - 1]->SetVictim1IP(ipv4))
			print_msg_and_abort("Invalid IP address for victim1!");
		if (!m_spoofTuples[index - 1]->SetVictim1MAC(mac))
			print_msg_and_abort("Invalid MAC address for victim1!");
		m_spoofTuples[index - 1]->SetInterval(m_interval);
		m_spoofTuples[index - 1]->SetProtocolType(m_protocol);
		if (!m_spoofTuples[index - 1]->SetInterface(m_interface))
			print_msg_and_abort("Invalid interface name!");
	}
	else {
		if (!m_spoofTuples[index - 1]->SetVictim2IP(ipv4))
			print_msg_and_abort("Invalid IP address for victim2!");
		if (!m_spoofTuples[index - 1]->SetVictim2MAC(mac))
			print_msg_and_abort("Invalid MAC address for victim2!");
	}
}
