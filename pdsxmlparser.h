/*
 * pdsxmlparser.h
 *
 *  Created on: Feb 21, 2017
 *      Author: jose
 */

#ifndef PDSXMLPARSER_H_
#define PDSXMLPARSER_H_

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "interface.h"

class PDSXMLParser {
	public:
		PDSXMLParser();
		virtual ~PDSXMLParser();

		static void DumpScan(const Interface *inface, const std::string filename);
		static void ChooseVictimPairs(const std::string inFile, const std::string outFile);

	private:
		static std::string UserInput();
		static bool ValidateInput(std::string input, int maxIndex, std::vector<unsigned int> &out);
		static void GroupPairs(xmlNodePtr rootNode, std::vector<unsigned int> pairs);
};

#endif /* PDSXMLPARSER_H_ */
