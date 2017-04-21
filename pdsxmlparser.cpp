/*
 * pdsxmlparser.cpp
 *
 *  Created on: Feb 21, 2017
 *      Author: Jan Profant
 */

#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>
#include <string>

#include "pdsxmlparser.h"
#include "errormsg.h"

/**
 * @brief Class constructor.
 */
PDSXMLParser::PDSXMLParser() {
}

/**
 * @brief Class destructor.
 */
PDSXMLParser::~PDSXMLParser() {
}

/**
 * @brief Dumps result of scan to XML file.
 */
void PDSXMLParser::DumpScan(const Interface *inface, const std::string filename) {
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL, node = NULL;/* node pointers */
	char mac[20];
	const unsigned char *p_mac;
	char ipv4[16];

#ifdef DEBUG
	std::cout << "Number of hosts: " << inface->m_hosts.size() << std::endl;
#endif

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "devices");
	xmlDocSetRootElement(doc, root_node);
	for (auto &i : inface->m_hosts) {
		node = xmlNewChild(root_node, NULL, BAD_CAST "host", NULL);
		p_mac = i->GetMAC();
		sprintf(mac, "%02x%02x.%02x%02x.%02x%02x",
				p_mac[0], p_mac[1], p_mac[2],
				p_mac[3], p_mac[4], p_mac[5]);
		xmlNewProp(node, BAD_CAST "mac", BAD_CAST mac);
		for (auto &x : i->GetIPv4()) {
			sprintf(ipv4, "%u.%u.%u.%u",
					x[0], x[1],
					x[2], x[3]);
			xmlNewChild(node, NULL, BAD_CAST "ipv4", BAD_CAST ipv4);
		}
	}
	xmlSaveFormatFileEnc(filename.c_str(), doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
}

/**
 * @brief Chooses pairs of victims and dump them.
 */
void PDSXMLParser::ChooseVictimPairs(const std::string inFile, const std::string outFile) {
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL, node = NULL, dnode = NULL;/* node pointers */
	unsigned int i = 0;
	std::vector<std::string> ipv4, ipv6;
	xmlChar *to_free = NULL;

	if ((doc = xmlReadFile(inFile.c_str(), NULL, 0)) == NULL) {
		print_msg_and_abort("ERROR: Can not parse input XML file!");
	}
	root_node = xmlDocGetRootElement(doc);
	for (node = root_node->children; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE && strcmp((char *)node->name, "host") == 0) {
			for (dnode = node->children; dnode; dnode = dnode->next) {
				if (dnode->type == XML_ELEMENT_NODE) {
					to_free = xmlNodeGetContent(dnode);
					std::string node_content((char *)to_free);
					if (strcmp((char *)dnode->name, "ipv4") == 0)
						ipv4.push_back(node_content);
					if (strcmp((char *)dnode->name, "ipv6") == 0)
						ipv6.push_back(node_content);
					xmlFree(to_free);
				}
			}
			to_free = xmlGetProp(node, node->properties->name);
			std::cout << "Index: " << i << ", mac: " << (char *)to_free;
			xmlFree(to_free);
			std::cout << ", ipv4:";
			for (auto &addr : ipv4)
				std::cout << " " << addr;
			std::cout << ", ipv6:";
			for (auto &addr : ipv6)
				std::cout << " " << addr;
			std::cout << std::endl;
			ipv4.clear();
			ipv6.clear();
			i++;
		}
	}
	std::string input;
	std::vector<unsigned int> tuples;
	bool isValid = true;
	do
	{
		if (!isValid) {
			std::cout << "WARNING: Incorrect user input!" << std::endl;
		}
		input = UserInput();
	} while (!(isValid = ValidateInput(input, i, tuples)));

	GroupPairs(root_node, tuples);
	xmlSaveFormatFileEnc(outFile.c_str(), doc, "UTF-8", 1);

	xmlFree(root_node);
	xmlFreeDoc(doc);
	xmlCleanupParser();
}

/**
 * @brief Requests user input.
 */
std::string PDSXMLParser::UserInput() {
	std::string input;
	std::cout << "Please, give me pairs (indexes of hosts) delimited by comma and terminated by comma." << std::endl
			<< "Example: 0 1, 2 3," << std::endl;
	std::getline(std::cin, input);
	return input;
}

/**
 * @brief Validates user input.
 */
bool PDSXMLParser::ValidateInput(std::string input, int maxIndex, std::vector<unsigned int> &out) {
	size_t pos = input.find(",");
	if (pos == std::string::npos)
		return false;

	std::string token;
	int i;

	do {
		std::istringstream iss(input.substr(0, pos));
		unsigned int num = 0;
		while (iss >> i) {
			std::cout << i << " " << maxIndex << std::endl;
			if (num < 2 && i < maxIndex)
				out.push_back(i);
			else
				return false;
			num++;
		}
		if (num != 2)
			return false;
		input.erase(0, pos + 1);
	} while ((pos = input.find(",")) != std::string::npos);

	std::set<unsigned int> outSet(out.begin(), out.end());
	if (outSet.size() != out.size())
		return false;

	return true;
}

/**
 * @brief Adds group attributes to XML.
 */
void PDSXMLParser::GroupPairs(xmlNodePtr rootNode, std::vector<unsigned int> pairs) {
	xmlNodePtr node;
	unsigned int i = 0;
	for (node = rootNode->children; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE && strcmp((char *)node->name, "host") == 0) {
			unsigned int pos = std::find(pairs.begin(), pairs.end(), i) - pairs.begin();
			if (pos < pairs.size()) {
				std::string tupIndex = SSTR(pos / 2 + 1);
				tupIndex = "victim-pair-" + tupIndex;
				xmlSetProp(node, (const xmlChar*)"group", (const xmlChar*)tupIndex.c_str());
			}
			i++;
		}
	}
}
