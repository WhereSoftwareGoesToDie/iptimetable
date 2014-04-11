#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ipspace.h"

#if _POSIX_C_SOURCE >= 200809L
#define SCANF_ALLOCATE_STRING_FLAG "m"
#elif defined(__GLIBC__) && (__GLIBC__ >= 2)
#define SCANF_ALLOCATE_STRING_FLAG "a"
#else
#error "Please let us have POSIX.1-2008, glibc, or a puppy"
#endif
#define SCANF_ALLOCATE_STRING "%" SCANF_ALLOCATE_STRING_FLAG "s"

#undef DEBUG

/* Read in a list of networks with netmasks from a file,
 * and return a linked list of networkaddr_ll_t
 *
 * Caller should call free_ipspace_list to free the list when
 * done
 */
networkaddr_ll_t * read_ipspace_file(char *pathname) {
	char *network = NULL;
	char *netmask = NULL;
	networkaddr_ll_t *head = NULL;

	FILE *infile = fopen(pathname, "r");
	if (infile == NULL)
		return perror(pathname), NULL;

	while (fscanf(infile,
			" %" SCANF_ALLOCATE_STRING_FLAG "[0123456789.]"
			"/" SCANF_ALLOCATE_STRING, &network,&netmask) == 2) {
		networkaddr_ll_t * entry = malloc(sizeof(networkaddr_ll_t));
		if (entry == NULL)
			return perror("malloc"),NULL;
		struct in_addr a;
		if (inet_aton(network, &a) == 0) {
			fprintf(stderr,"Invalid ipspace network '%s'. Skipping\n",network);
			free(entry); free(network); free(netmask); continue;
		}
		entry->network = a.s_addr;
		struct in_addr b;
		if (inet_aton(netmask, &b) == 0) {
			fprintf(stderr,"Invalid ipspace network '%s'. Skipping\n",network);
			free(entry); free(network); free(netmask); continue;
		}
		entry->netmask = b.s_addr;
		if (ntohl( entry->netmask ) <= 24) {
			/* Prefix length not netmask */
			entry->netmask = ntohl(entry->netmask);
			entry->netmask = ~((1<<(32- entry->netmask )) - 1);
			entry->netmask = htonl(entry->netmask);
		}
		a.s_addr = entry->network;
		b.s_addr = entry->netmask;
#ifdef DEBUG
		fprintf(stderr,"Added to ipspace: %s/",
				inet_ntoa(a));
		fprintf(stderr,"%s\n",inet_ntoa(b));
#endif

		entry->next = head;
		head = entry;
		free(network); free(netmask);
	}
	fclose(infile);
	return head;
}

/* Free memory for a linked list allocated by read_ipspace_list
 */
void free_ipspace_list(networkaddr_ll_t *ipspace) {
	while (ipspace != NULL) {
		networkaddr_ll_t *next = ipspace->next;
		free(ipspace);
		ipspace = next;
	}
}

/*
 * Given a network/netmask and an IP address, return the next IP address
 * in the network.
 *
 * Returns 0 when the passed ip address is the last one in the network
 */
inline int get_next_ip_address(in_addr_t ip, networkaddr_ll_t prefix) {
	ip = htonl(ntohl(ip)+1);
	return ((ip & prefix.netmask) == prefix.network) ? ip : 0;
}

/* returns 1 if it is in the ipspace, 0 if it is not, or -1
 * if it is not a well-formed address
 */
inline int is_address_in_ipspace(char *ipaddr , networkaddr_ll_t *ipspace) {
	struct in_addr addr;
	if (inet_aton(ipaddr, &addr) == 0) return -1;
	/* everything is ipspaceed by default if there is no ipspace */
	if (ipspace == NULL) return 1;
	for (; ipspace != NULL; ipspace = ipspace->next) {
		if ((addr.s_addr & ipspace->netmask) == ipspace->network)
			return 1;
	}
	return 0;
}
