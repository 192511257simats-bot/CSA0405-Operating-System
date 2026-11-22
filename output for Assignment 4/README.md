# CSA0405-<RegisterNumber>-Assignment4
Name: <your name>    Register number: <your register number>

Environment: Ubuntu 24.04.5 LTS, gcc 13.3.0. Compile: `gcc -std=c11 -Wall -Wextra -o X X.c` (add `-lm` for sim.c / starvation.c / stream_sim.c; Task 6 also `-O2`).

| Folder | Programs | Run | Result |
|---|---|---|---|
| Task02 | alloc.c, alloc_extra.c, costs.c | `./alloc` `./alloc_extra` `./costs` | alloc output matches expected; 15-block file fails contiguous, works indexed; last block of 4 GB = 1 / 1,048,576 / 3 reads |
| Task03 | inspect_fs.sh | `./inspect_fs.sh` | ext4 image: block 4096, inode 256, has_journal, extent |
| Task04 | sstf.c, six_algorithms.c, sim.c, starvation.c | run each | FCFS 640, SSTF 236, SCAN 236, C-SCAN 382, LOOK 208, C-LOOK 322; SSTF starves cyl 183 (5034 ms) |
| Task06 | stream_sim.c | `./stream_sim` | tiered design p99 147 ms vs HDD-only 255 ms at 10,000 streams |

Put your own terminal screenshots in each task folder (`screenshots/` holds the images used in the report).
