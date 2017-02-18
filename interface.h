/*
 * Interface.h
 *
 *  Created on: Feb 18, 2017
 *      Author: jose
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string>

/**
 * Class Interface represent current active interface.
 */
class Interface {
	public:
		Interface();
		virtual ~Interface();

		/**
		 * Class members.
		 */
		int m_index;
		std::string m_name;
		std::string	m_ip;
		std::string m_mac;

};

#endif /* INTERFACE_H_ */
