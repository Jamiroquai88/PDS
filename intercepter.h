/*
 * intercept.h
 *
 *  Created on: Apr 20, 2017
 *      Author: Jan Profant
 */

#ifndef INTERCEPTER_H_
#define INTERCEPTER_H_

#define ETH_FRAME_SIZE 1522

#include "interface.h"
#include "host.h"

#include <map>

/**
 * @brief Intercepts spoofed communication.
 */
class Intercepter {
	public:
		Intercepter();
		virtual ~Intercepter();

		void SetInterface(std::string inface);
		void SetFile(std::string f);

		void Free();

		void *Start(void);
		static void *Start_helper(void *context) 									{ return ((Intercepter *)context)->Start(); }

		const std::map<unsigned int, std::vector<Host*> >& GetHostsMap() const 		{ return m_hostsMap; 						}

	protected:
		void AddHost(unsigned int index, std::string mac);
		void AddAddress(unsigned int index, std::string ip, bool ipv4);
		int OpenSocket();
		void InitSLL(sockaddr_ll *sll);

	private:
		Interface *mp_interface;
		std::map<unsigned int, std::vector<Host *>> m_hostsMap;
};

#endif /* INTERCEPTER_H_ */
