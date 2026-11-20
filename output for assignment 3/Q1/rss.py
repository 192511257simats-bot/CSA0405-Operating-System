import resource, subprocess, sys
for b in ["./bank_static","./bank_dyn"]:
    subprocess.run([b],stdout=subprocess.DEVNULL)
    print(b, "peak RSS (children) kB:", resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss)
    # separate process each so maxrss isn't cumulative
    r=subprocess.run(["python3","-c",f"import resource,subprocess;subprocess.run(['{b}'],stdout=subprocess.DEVNULL);print(resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss)"],capture_output=True,text=True)
    print("  isolated maxrss kB:", r.stdout.strip())
