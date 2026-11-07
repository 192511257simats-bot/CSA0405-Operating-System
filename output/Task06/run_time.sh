#!/bin/bash
# user vs kernel time for the 8 MiB copy (bash built-in time)
TIMEFORMAT='real %3Rs  user %3Us  sys %3Ss'
for b in 1 512 4096 65536; do printf 'buffer %6d bytes : ' $b; time ./bench_copy sys big.dat o.dat $b; done
rm -f o.dat
