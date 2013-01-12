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
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include "sockwrap.h"
#include "icmp.h"
#include "ip_list.h"

#define TIMEOUT 1000  /* Time of waiting for packets with last set TTL in miliseconds (= 1 second) */
#define TTL_LIMIT 30
#define REQUESTS_PER_TTL 3
#define BUFFER_SIZE 128


/* Returns the difference between two times in miliseconds. struct timeval member tv_sec holds the number of
   seconds (1 s = 1000 ms) and member tv_usec holds the number of microseconds (1 us = 1/1000 ms) */
double timeDifference(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_usec - start.tv_usec)/1000.0;
}


int main(int argc, char* argv[]) {
    
    if(argc != 2) { printf("Usage: ./traceroute <IP_address>\n"); exit(1); }
    
    struct sockaddr_in remoteAddr;
    bzero(&remoteAddr, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    Inet_pton(AF_INET, argv[1], &remoteAddr.sin_addr);  // An error message is displayed if the given IP address is invalid
              
    int pid = getpid();  // get process id for later identification
    
    int sockId = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);  // acquire socket, store socket's id
    struct timeval begin, current;
    begin.tv_sec = 0;
    begin.tv_usec = 1000;  // (= 1 ms)
    Setsockopt(sockId, SOL_SOCKET, SO_RCVTIMEO, &begin, sizeof(begin));  // set time limit of socket's waiting for a packet
    
    
    char icmpRequestBuffer[BUFFER_SIZE], replyBuffer[BUFFER_SIZE];  // place in memory for our ICMP requests and received IP packets

    struct icmp *icmpRequest = (struct icmp *) icmpRequestBuffer;
    icmpRequest->icmp_type = ICMP_ECHO;
    icmpRequest->icmp_code = htons(0);  // htons(x) returns the value of x in TCP/IP network byte order
    icmpRequest->icmp_id = htons(pid);    
    
    int ttl, sequence = 0, repliedPacketsCnt, i;
    bool stop = 0;  // set to true, when echo reply has been received
    double elapsedTime;  // variable used to compute the average time of responses
    struct timeval sendTime[REQUESTS_PER_TTL];  // send time of a specific packet
    ip_list *ipsThatReplied;  // list of IPs that replied to echo request
    
    for(ttl=1; ttl<=TTL_LIMIT; ttl++) {
		repliedPacketsCnt = 0;
		elapsedTime = 0.0;
		ipsThatReplied = createIpList();

		for(i=1; i<=REQUESTS_PER_TTL; i++) {
			Setsockopt(sockId, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));  // set TTL of IP packet that is being sent
			icmpRequest->icmp_seq = htons(++sequence);  // set sequence number, for later identification
			
			icmpRequest->icmp_cksum = 0;
			icmpRequest->icmp_cksum = in_cksum((uint16_t*) icmpRequest, ICMP_HEADER_LEN, 0);
			
			gettimeofday(&sendTime[(sequence-1) % REQUESTS_PER_TTL], NULL);
			Sendto(sockId, icmpRequestBuffer, ICMP_HEADER_LEN, 0, &remoteAddr, sizeof(remoteAddr));
		}
	
		gettimeofday(&begin, NULL);  // get time after sending the packets
	
		while(repliedPacketsCnt < REQUESTS_PER_TTL) {
		  
			int RecvRetVal = Recvfrom(sockId, replyBuffer, BUFFER_SIZE, 0, 0, 0);  // wait 1 ms for a packet (at most)
			gettimeofday(&current, NULL);
			
			if(RecvRetVal < 0) {
				if(timeDifference(begin, current) > TIMEOUT) break;
				continue;
			}
			
			struct ip *reply = (struct ip *) replyBuffer;
			
			if(reply->ip_p != IPPROTO_ICMP) continue;  // Check packet's protocol (if it's ICMP)
			
			struct icmp *icmpHeader = (struct icmp *) (replyBuffer + reply->ip_hl*4);  // we "extract" the ICMP header from the IP packet
			
			if(icmpHeader->icmp_type != ICMP_ECHOREPLY && 
			  !(icmpHeader->icmp_type == ICMP_TIME_EXCEEDED && icmpHeader->icmp_code == ICMP_EXC_TTL)) continue;
			// If the packet's type is neither echo reply, nor time exceeded because of TTL depletion
			
			if(icmpHeader->icmp_type == ICMP_TIME_EXCEEDED)
			icmpHeader = (struct icmp *) (icmpHeader->icmp_data + ((struct ip *) (icmpHeader->icmp_data))->ip_hl*4);
			// if we got time_exceeded packet, shift icmpHeader to the copy of our request
			
			if(ntohs(icmpHeader->icmp_id) != pid || sequence - ntohs(icmpHeader->icmp_seq) >= REQUESTS_PER_TTL) continue;
			// is icmp_id equal to our pid and it's one of the latest (three) packets sent?
			
			elapsedTime += timeDifference(sendTime[(ntohs(icmpHeader->icmp_seq)-1) % REQUESTS_PER_TTL], current);
			insert(ipsThatReplied, reply->ip_src);
			repliedPacketsCnt++;
			
			if(icmpHeader->icmp_type == ICMP_ECHOREPLY) stop = 1;
		}
	
		// Output
		printf("%2d. ", ttl);
		if(repliedPacketsCnt == 0) { printf("*\n"); continue; }
		printIpList(ipsThatReplied);
		destroyIpList(ipsThatReplied);
			
		if(repliedPacketsCnt == REQUESTS_PER_TTL) printf("%.1f ms\n", elapsedTime / repliedPacketsCnt);
		else printf("\t???\n");
	
		if(stop == 1) break;
    }
    return 0;
}
