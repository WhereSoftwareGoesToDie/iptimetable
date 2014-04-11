#!/bin/bash
#
#
# Create a new sparse iptimetable database
#
# e.g. ./make_new_iptimetable.sh testdb ip_space.txt $(date +%s -d "1 Jan 2014") 365
#

DBNAME=$1
IPSPACEFILE=$2
STARTTIMESTAMP=$3
DAYS=$4

if [ -z "$DBNAME" ] || [ -z "$IPSPACEFILE" ] || [ -z "$STARTTIMESTAMP" ] || [ -z "$DAYS" ]; then
	echo "$0 <database filename> <ipspacefile> <start timestamp> <days>"
	echo
	echo 'e.g. ./make_new_iptimetable.sh testdb ip_space.txt $(date +%s -d "1 Jan 2014") 365'
	exit 1
fi

# Figure out how big the file needs to be
ROWS=$(./dump_ipspace $IPSPACEFILE | wc -l)

# One entry for every 10 minutes
COLS=$(($DAYS * 24 * 6))

# Each entry is 8 bytes wide
TABLESIZE=$((8 * $COLS * $ROWS))

echo -n Start:
date -d @${STARTTIMESTAMP} || exit 1
echo "Creating sparse $DBNAME ($ROWS IPs. $COL entries per IP. $TABLESIZE bytes)"
dd if=/dev/zero of="$DBNAME" bs=1 count=1 seek=$(( ${TABLESIZE} - 1))

echo "Copying $IPSPACEFILE to ${DBNAME}.ipspace"
cp $IPSPACEFILE ${DBNAME}.ipspace

echo "Writing dimensions"
echo $ROWS $COLS > "${DBNAME}.dims"


chmod a-w "${DBNAME}.ipspace" "${DBNAME}.dims"
