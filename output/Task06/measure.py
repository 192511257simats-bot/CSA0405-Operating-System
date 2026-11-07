#!/usr/bin/env python3
"""Task 6/10 measurement driver: counts via mystrace -c (256 KiB file), times via wait4 rusage (8 MiB file)."""
import os, subprocess, re, time, json, sys
def run(cmd, trace=False):
    t0=time.perf_counter(); p=subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    _, st, ru = os.wait4(p.pid, 0); wall=time.perf_counter()-t0
    err=p.stderr.read().decode(); return wall, ru.ru_utime, ru.ru_stime, err
res={"counts":[], "times":[]}
for b in [1,16,512,4096,65536]:
    _,_,_,err=run(["../mystrace","-c","./bench_copy","sys","small.dat","o.dat",str(b)])
    m=lambda n: int((re.search(r"\s(\d+)\s+(?:\d+\s+)?%s\n"%n, err) or re.search(r"(\d+)\s+%s"%n, err)).group(1))
    rd=int(re.search(r"\s+(\d+)\s+read\n",err).group(1)); wr=int(re.search(r"\s+(\d+)\s+write\n",err).group(1))
    tot=int(re.search(r"\s(\d+)\s+\d+\s+total",err).group(1))
    res["counts"].append((b,rd,wr,tot))
for mode,b in [("sys",1),("sys",16),("sys",512),("sys",4096),("sys",65536),("stdio",1),("stdio",4096),("mmap",0)]:
    best=None
    for _ in range(3):
        r=run(["./bench_copy",mode,"big.dat","o.dat",str(b)])
        if best is None or r[0]<best[0]: best=r
    res["times"].append((mode,b)+tuple(round(x,4) for x in best[:3]))
# library buffering: syscalls for stdio (fread/fwrite of 1 byte? use 4096 and 16 via setvbuf default)
for mode,b in [("stdio",16),("stdio",4096),("mmap",0)]:
    _,_,_,err=run(["../mystrace","-c","./bench_copy",mode,"small.dat","o.dat",str(b)])
    rd=re.search(r"\s+(\d+)\s+read\n",err); wr=re.search(r"\s+(\d+)\s+write\n",err); tot=re.search(r"\s(\d+)\s+\d+\s+total",err)
    res["counts"].append((mode+str(b), int(rd.group(1)) if rd else 0, int(wr.group(1)) if wr else 0, int(tot.group(1))))
os.remove("o.dat"); json.dump(res,open("results.json","w"),indent=1)
print("SYSTEM-CALL COUNTS (256 KiB file)   [buffer, read(), write(), all syscalls]")
for c in res["counts"]: print("  ",c)
print("TIMES (8 MiB file)   [mode, buf, real s, user s, sys s]")
for t in res["times"]: print("  ",t)
