#!/bin/bash

TARGET_FILE=$1
LINE_COUNT=$2

echo "8bit Character ROM Viewer"
if [ $# -eq 0 ]
then 
	echo "usage :: .//viewer.sh target_file (line_count)"
else
	if [ -z "$2" ]
	then
		LINE_STRING=""
	else
		LINE_STRING="-l ${LINE_COUNT} "
	fi
	echo
	echo "  ${TARGET_FILE}"
	echo
	xxd -b -c 1 $LINE_STRING$TARGET_FILE | tr 0 " " | tr 1 "#"
fi
echo $LINE_STRING
