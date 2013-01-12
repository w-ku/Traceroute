CC = gcc 
CFLAGS = -Wall -W -Wshadow -std=gnu99 
TARGETS = traceroute
 
all: $(TARGETS)

traceroute: traceroute.o sockwrap.o icmp.o ip_list.o

clean: 
	rm -f *.o	

distclean: clean
	rm -f $(TARGETS)
