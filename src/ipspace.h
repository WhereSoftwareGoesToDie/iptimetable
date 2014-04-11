#ifndef __IPSPACE_H
#define __IPSPACE_H

#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
	in_addr_t network;
	in_addr_t netmask;
	void * next;
} networkaddr_ll_t;



/* Read in a list of networks with netmasks from a file,
 * and return a linked list of networkaddr_ll_t
 *
 * Caller should call free_ipspace_list to free the list when
 * done
 */
networkaddr_ll_t * read_ipspace_file(char *pathname);

/* Free memory for a linked list allocated by read_ipspace_list
 */
void free_ipspace_list(networkaddr_ll_t *ipspace);

/*
 * Given a network/netmask and an IP address, return the next IP address
 * in the network.
 *
 * Returns 0 when the passed ip address is the last one in the network
 */
inline int get_next_ip_address(in_addr_t ip, networkaddr_ll_t prefix);

/* returns 1 if it is in the ipspace, 0 if it is not, or -1
 * if it is not a well-formed address
 */
inline int is_address_in_ipspace(char *ipaddr , networkaddr_ll_t *ipspace);

#endif
