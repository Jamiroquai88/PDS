/*
 * arpheader.h
 *
 *  Created on: Feb 19, 2017
 *      Author: jose
 */

#ifndef PROTHEADER_H_
#define PROTHEADER_H_

#define ETH_ALEN	6		//Octets in one ethernet address

/**
 * @brief Structure responsible for constructing ARP header.
 * Based on: http://divingintolinux.sanupdas.com/?p=239
 */
struct arp_header {
	unsigned short 	hrd;		        //hardware address type
	unsigned short 	proto;				//protocol adress type
	unsigned char 	hrd_add_len;		//hardware address length
	unsigned char 	proto_add_len;		//Protocol adress length
	unsigned short 	op;					//Operation
	unsigned char 	smac[ETH_ALEN];		//source MAC (Ethernet Address)
	unsigned char 	sip[4];				//source IP
	unsigned char 	dmac[ETH_ALEN];		//destination MAC (Ethernet Address)
	unsigned char 	dip[4];				//destination IP
	char 			pad[18];			//Padding, ARP-Requests are quite small (<64)
};

/**
 * @brief Structure for ethernet header.
 */
struct eth_header {
	unsigned char 		dmac[ETH_ALEN];
	unsigned char		smac[ETH_ALEN];
	unsigned short int	ethType;
};

/**
 * @brief Structure for whole ARP packet.
 */
struct arp_packet {
	struct eth_header;
	struct arp_header;
};

#endif /* PROTHEADER_H_ */
