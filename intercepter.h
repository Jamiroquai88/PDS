/*
 * intercept.h
 *
 *  Created on: Apr 20, 2017
 *      Author: Jan Profant
 */

#ifndef INTERCEPTER_H_
#define INTERCEPTER_H_

#include "interface.h"
#include "host.h"

#include <map>

class Intercepter {
	public:
		Intercepter();
		virtual ~Intercepter();

		void SetInterface(std::string inface);
		void SetFile(std::string f);

		void Start();
		void Free();

	protected:
		void AddHost(unsigned int index, std::string mac);
		void AddAddress(unsigned int index, std::string ip, bool ipv4);
		int OpenSocket();
		void InitSSL(struct sockaddr_ll *sll);

	private:
		Interface *mp_interface;
		std::map<unsigned int, std::vector<Host *>> m_hostsMap;
};

#endif /* INTERCEPTER_H_ */
