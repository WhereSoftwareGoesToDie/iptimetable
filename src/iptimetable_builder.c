/* Build a histogram of time intervals and populate it from a vaultaire source
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <lisant/lisant.h>



int main(int argc, char **argv) {
	int i;

	if (argc < 7) {
		fprintf(stderr, "usage:\tcreate_histogram <start timestamp> <seconds per bin> <number of bins> <vaultaire server> "\
			"<origin> <source>\n");
		return -1;
	}

	++argv;
	uint64_t start_timestamp = atol(*(argv++));
	uint64_t seconds_per_bin = atol(*(argv++));
	uint64_t bincount = atol(*(argv++));
	char * vaultaire_server = *(argv++);
	char * origin = *(argv++);
	char * source = *(argv++);

#define NANOSECONDS	1000000000

#define END_TIMESTAMP	(start_timestamp + (seconds_per_bin * bincount))
#define START_VAULT_TIMESTAMP	(start_timestamp * NANOSECONDS)
#define END_VAULT_TIMESTAMP	(END_TIMESTAMP * NANOSECONDS)

#define VAULT_TIMESTAMP_WITHIN_BOUNDS(_vault_timestamp) \
	( ((_vault_timestamp)/NANOSECONDS >= start_timestamp) && \
	  ((_vault_timestamp)/NANOSECONDS < END_TIMESTAMP ) )

#define BIN_FOR_VAULT_TIMESTAMP(_vault_timestamp) \
       	( (((_vault_timestamp)/NANOSECONDS) - start_timestamp ) / seconds_per_bin )

#define TIMESTAMP_FOR_BIN(_bin) (start_timestamp + (_bin)*seconds_per_bin)

	uint64_t *bins = calloc(bincount, sizeof(uint64_t));
	if (bins == NULL)
		return perror("calloc"), 1;


	void *vaultaire = init_vaultaire_reader();
	void *con = vaultaire_reader_connect(vaultaire, vaultaire_server);
	if (con == NULL) return perror("vaultaire_reader_connect"), 1;

	uint64_t *timestamps;
	uint64_t *values;
	int points_read = vaultaire_get_numeric_source(con,
			&timestamps, &values, origin, source,
			START_VAULT_TIMESTAMP, END_VAULT_TIMESTAMP);

	if (points_read < 0) return perror("vaultaire_get_numeric"), 1;

	for (i=0; i<points_read; ++i)
		if (VAULT_TIMESTAMP_WITHIN_BOUNDS(timestamps[i]))
			bins[BIN_FOR_VAULT_TIMESTAMP(timestamps[i])] += values[i];

	/* output */
	for (i=0; i<bincount; ++i)
		printf("%lu %lu\n", TIMESTAMP_FOR_BIN(i), bins[i]);

	free(timestamps); free(values); free(bins);
	vaultaire_reader_close(con); 
	vaultaire_reader_shutdown(vaultaire);

	return 0;
}

