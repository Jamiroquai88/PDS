/*
 * pdsxmlparser.cpp
 *
 *  Created on: Feb 21, 2017
 *      Author: jose
 */

#include "pdsxmlparser.h"

PDSXMLParser::PDSXMLParser() {
}

PDSXMLParser::~PDSXMLParser() {
}

void PDSXMLParser::DumpScan(const Interface *inface, const std::string filename) {
	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;/* node pointers */
	char mac[20];
	char ipv4[16];

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "devices");
	xmlDocSetRootElement(doc, root_node);
	for (auto &i : inface->m_hosts) {
		node = xmlNewChild(root_node, NULL, BAD_CAST "host", NULL);
		sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", inface->m_mac);
		xmlNewProp(node, BAD_CAST "mac", BAD_CAST mac);
		sprintf(ipv4, "%u.%u.%u.%u", inface->m_ip);
		xmlNewChild(node, NULL, BAD_CAST "ipv4", BAD_CAST ipv4);
	}
	xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
}

