/*
 * pdsxmlparser.h
 *
 *  Created on: Feb 21, 2017
 *      Author: jose
 */

#ifndef PDSXMLPARSER_H_
#define PDSXMLPARSER_H_

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "interface.h"

class PDSXMLParser {
public:
	PDSXMLParser();
	virtual ~PDSXMLParser();

	static void DumpScan(const Interface *inface, const std::string filename);
};

#endif /* PDSXMLPARSER_H_ */
