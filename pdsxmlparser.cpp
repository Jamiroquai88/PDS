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
	char buff[256];
	int i, j;

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "devices");
	xmlDocSetRootElement(doc, root_node);
	xmlNewChild(root_node, NULL, BAD_CAST "host",
			BAD_CAST "content of host");
	xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
}

