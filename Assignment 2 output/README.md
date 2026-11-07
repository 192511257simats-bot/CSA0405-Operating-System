# CSA0405 – Operating Systems – Assignment 2 (CO2)

- **Name:** <your name>
- **Register number:** <your register number>
- **Compiler:** gcc 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1)
- **OS:** Ubuntu 24.04.5 LTS (Linux 6.18), 2 CPU cores
- **Compile command:** `gcc -std=c11 -Wall -Wextra -pthread file.c -o file` (benchmarks add `-O2`; Q4 `bench_ipc.c` adds `-lrt`)

| Task | Programs | How to run | Result |
|---|---|---|---|
| Q1 | fork_demo.c, bench_q1.c | `cd Q1 && ./run_q1.sh` | fork + ps + /proc evidence; process create 106.6 µs vs thread 39.9 µs; design: worker-process pool |
| Q2 | rr_complete.c, sched_all.c, rr_study.c, mlfq.c | `gcc rr_complete.c -o rr && ./rr` (add `-DQUANTUM=1` / `20`) | RR output matches expected; avg WT FCFS 8.75, SJF 7.75, SRTF 6.50, RR(3) 13.50; MLFQ recommended |
| Q3 | plates.c, procs.c | `gcc -O1 -pthread plates.c -o plates && ./plates` | speed-up ≈ 2.0x on 2 cores; unprotected counter loses updates |
| Q4 | pipe_complete.c, shm_sem.c, bench_ipc.c | `gcc -DNO_CLOSE pipe_complete.c` shows the hang; `gcc -O2 -pthread bench_ipc.c -lrt` | pipe output matches expected; 100 MB: pipe 55.5 ms, shm 6.5 ms, mq 28.9 ms |
| Q5 | bb_complete.c, bb_multi.c, peterson.c, classic.c, lockgran.c, atomic_spin.c | `gcc -pthread bb_complete.c && for i in $(seq 100); do ./a.out; done \| sort \| uniq -c` | sum 210 in 100/100 runs; lock per coach 3.7x faster than global |
| Q6 | banker_complete.c, detect.c, bench_banker.c | `gcc banker_complete.c && ./a.out` | safe sequence P1 P3 P4 P0 P2; P0 denied; O(m·n²) confirmed |

Each folder contains `output*.txt` (captured program output) and `screenshots/`.
