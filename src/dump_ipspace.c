/*
 * Unit test helper:
 *
 * Print every IPv4 address in every network listed in an ipspace.txt
 */

#include <stdio.h>
#include "ipspace.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "dump_ipspace <ipspace.txt>\n");
		return 1;
	}

	networkaddr_ll_t *networks = read_ipspace_file(argv[1]);
	if (networks == NULL) return -1;

	networkaddr_ll_t *iter;
	for (iter=networks; iter != NULL; iter=iter->next) {
		in_addr_t ip = iter->network;

		do {
			printf("%s\n", inet_ntoa((struct in_addr){ip}));
			ip = get_next_ip_address(ip, *iter);
		}  while (ip != 0);
	}

	free_ipspace_list(networks);
	return 0;
}

