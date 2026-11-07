# CSA04 Assignment 1 – Smart Hospital OS design
Name: [YOUR NAME]   Register No: [REGISTER NUMBER]
Compiler: gcc 13.3.0 | OS: Ubuntu 24.04 LTS (x86-64)
Compile: `gcc -std=c11 -Wall -Wextra -pthread file.c -o name`
| Task | Run | Result |
|---|---|---|
| 5 | `./sysread`, `strace ./sysread` | openat/read/write/close sequence shown |
| 6 | `./copy_file` | 10001 / 21 / 4 read() calls = expected |
| 7 | `./shell` | 42, status 0, status 3 = expected |
| 10 | `./bench_proc` | fork 47 us, thread 6.8 us per creation |
`tools/mystrace.c` is a ptrace-based strace clone (use real `strace` if installed). Task06 test files: `head -c 8388608 /dev/urandom > big.dat; head -c 262144 big.dat > small.dat`.
