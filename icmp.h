/* 
	Author: Wojciech Kuprianowicz

	Author:
	Wojciech Kuprianowicz 

	License:
	This source code can be used freely by anyone and for any purpose. I do not take any responsibility for any damage caused through use of this software.

	Application name:
	Traceroute

	Current Version:
	1.0 (2012-03-24) (checked 2013-01-12)

	Description:
	Traceroute in C using raw sockets.
*/

#ifndef __ICMP_H
#define __ICMP_H

#define ICMP_HEADER_LEN 8


uint16_t in_cksum(uint16_t *addr, int len, int csum);


#endif

