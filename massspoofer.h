/*
 * massspoofer.h
 *
 *  Created on: Apr 20, 2017
 *      Author: Jan Profant
 */

#ifndef MASSSPOOFER_H_
#define MASSSPOOFER_H_

#include "spoofer.h"
#include "host.h"
#include "protheader.h"
#include "interface.h"

/**
 * @brief Handles mass spoofing specified in XML.
 */
class MassSpoofer {
	public:
		MassSpoofer();
		virtual ~MassSpoofer();

		void Start();
		void SetProtocolType(std::string t)					{ m_protocol = t; 		}
		void SetInterface(std::string inface);
		void SetInterval(unsigned int time)                 { m_interval = time; 	}
		bool SetFile(std::string f);
		void Free();
		void Add(unsigned int index, std::string ipv4, std::string mac);

	private:
		/**
		 * Class members.
		 */
		std::string m_protocol;
		Host::ip_address_type m_ipType;
		std::string m_interface;
		unsigned int m_interval;
		std::vector<Spoofer *> m_spoofTuples;
		std::vector<pthread_t *> m_threads;
};

#endif /* MASSSPOOFER_H_ */
