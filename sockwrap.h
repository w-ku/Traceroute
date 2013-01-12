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

#ifndef __SOCKWRAP_H_
#define __SOCKWRAP_H_


int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr_in *sa, socklen_t salen);
ssize_t Recvfrom (int fd, void *ptr, size_t nbytes, int flags, struct sockaddr_in *sa, socklen_t *salenptr);
void Sendto(int fd, const void *ptr, int nbytes, int flags, const struct sockaddr_in *sa, socklen_t salen);
void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

int Inet_pton(int family, const char *src, void *dst);

#endif
