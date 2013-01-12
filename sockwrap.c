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

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include "sockwrap.h"

#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(1); }
#define CUSTOM_ERROR_MESSAGE(str) { fprintf(stderr, "%s\n", str); exit(1); }

int Socket(int family, int type, int protocol)
{
	int	n;
	if ( (n = socket(family, type, protocol)) < 0)
		ERROR ("socket error");
	return n;
}

void Bind(int fd, const struct sockaddr_in *sa, socklen_t salen)
{
	if (bind(fd, (struct sockaddr*)sa, salen) < 0)
		ERROR ("bind error");
}

ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr_in *sa, socklen_t *salenptr)
{
	ssize_t	n = recvfrom(fd, ptr, nbytes, flags, (struct sockaddr*)sa, salenptr);
	if(n < 0 && errno != EAGAIN)
		ERROR ("recvfrom error");
	return n;
}

void Sendto(int fd, const void *ptr, int nbytes, int flags, const struct sockaddr_in *sa, socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, (struct sockaddr*)sa, salen) != nbytes)
		ERROR ("sendto error");
}

void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(sockfd,level,optname,optval,optlen) < 0) 
		ERROR ("setsockopt error");
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int n;
	if ((n = select(nfds,readfds,writefds,exceptfds,timeout)) < 0) 
		ERROR ("select error");
	return n;	
}


int Inet_pton(int family, const char *src, void *dst) {
    int n = inet_pton(family, src, dst);
    if(n == 0) CUSTOM_ERROR_MESSAGE("inet_pton error (invalid IP address)");
    if(n == -1) CUSTOM_ERROR_MESSAGE("inet_pton error (invalid address family)");
    return n;
}

