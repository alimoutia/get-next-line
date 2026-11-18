#!/bin/sh
# Makes the Nth malloc fail, for N = 1..80, one run per N.
# usage: ./failtest.sh "<your .c files, WITHOUT a main>" <input file> <BUFFER_SIZE> [function name]
# example: ./failtest.sh "gnl.c gnl_utils.c" text.md 5 gnl
# A run is GOOD when the program does not crash, leaks nothing, and prints only
# a prefix of the input made of whole lines.
srcs=$1; file=$2; bs=$3; fn=${4:-get_next_line}
cc -g -fsanitize=address -D BUFFER_SIZE=$bs -DGNL=$fn $srcs test_main.c malloc_fail.c \
	-Wl,--wrap=malloc -o failbin || exit 1
size=$(wc -c < $file)
bad=0
for n in $(seq 1 80); do
	FAIL_AT=$n ./failbin $file > fo.txt 2> fe.txt
	sz=$(wc -c < fo.txt)
	if ! head -c $sz $file | cmp -s - fo.txt; then
		echo "N=$n: output is NOT a prefix of the input"; bad=$((bad+1)); continue
	fi
	if [ $sz -gt 0 ] && [ $sz -lt $size ] && [ "$(tail -c 1 fo.txt | od -An -c | tr -d ' ')" != '\n' ]; then
		echo "N=$n: output stops in the middle of a line"; bad=$((bad+1)); continue
	fi
	if grep -q "ERROR" fe.txt; then
		echo "N=$n: sanitizer: $(grep -m1 -o 'ERROR: [A-Za-z]*' fe.txt)"; bad=$((bad+1))
	fi
done
echo "bad runs: $bad / 80"
