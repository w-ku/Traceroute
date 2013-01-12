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

#ifndef IP_LIST_H
#define IP_LIST_H

// A simple list structure that stores IP addresses (struct in_addr). Allows inserting new elements
// (without repetitions), printing the content and clearing the list.

typedef struct IP_LIST {
    struct in_addr value;
    struct IP_LIST *next;
} ip_list;


ip_list *createIpList();
void destroyIpList(ip_list *node);
int insert(ip_list *root, struct in_addr x);
void printIpList(ip_list *root);


#endif /* IP_LIST_H */
