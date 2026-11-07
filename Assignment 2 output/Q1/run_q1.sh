#!/bin/bash
# builds, runs fork_demo in background and observes it with ps and /proc
gcc -std=c11 -Wall -Wextra -o fork_demo fork_demo.c && gcc -std=gnu11 -O2 -Wall -Wno-unused-result -pthread -o bench_q1 bench_q1.c || exit 1
./fork_demo 4 6 > fork_out.txt &
sleep 2
PARENT=$!
echo "\$ ps -o pid,ppid,stat,wchan:12,rss,cmd --ppid $PARENT -p $PARENT"
ps -o pid,ppid,stat,wchan:12,rss,cmd --ppid $PARENT -p $PARENT
CH=$(pgrep -P $PARENT | head -1)
echo; echo "\$ grep -E 'Name|State|Pid|PPid|VmSize|VmRSS|voluntary' /proc/$CH/status"
grep -E 'Name|State|^Pid|PPid|VmSize|VmRSS|ctxt' /proc/$CH/status
echo; echo "\$ ls /proc/$CH | head -12"; ls /proc/$CH | head -12 | tr '\n' ' '; echo
wait
echo; echo "--- program output ---"; cat fork_out.txt
echo; echo "\$ ./bench_q1"; ./bench_q1
