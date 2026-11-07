/* mystrace.c - minimal strace / "strace -c" clone built on ptrace(2) (x86-64 Linux).
   Usage: ./mystrace [-c] prog [args...]   (trace output goes to stderr, like strace) */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>

static const struct { int n; const char *s; } names[] = {
 {0,"read"},{1,"write"},{2,"open"},{3,"close"},{4,"stat"},{5,"fstat"},{8,"lseek"},{9,"mmap"},
 {10,"mprotect"},{11,"munmap"},{12,"brk"},{13,"rt_sigaction"},{16,"ioctl"},{17,"pread64"},
 {21,"access"},{25,"mremap"},{39,"getpid"},{56,"clone"},{57,"fork"},{59,"execve"},{61,"wait4"},
 {63,"uname"},{72,"fcntl"},{87,"unlink"},{89,"readlink"},{158,"arch_prctl"},{202,"futex"},
 {218,"set_tid_address"},{228,"clock_gettime"},{231,"exit_group"},{257,"openat"},{262,"newfstatat"},
 {263,"unlinkat"},{273,"set_robust_list"},{302,"prlimit64"},{318,"getrandom"},{334,"rseq"},
 {435,"clone3"},{-1,NULL}};
static const char *nm(long n){ static char b[32]; for(int i=0;names[i].s;i++) if(names[i].n==n) return names[i].s; snprintf(b,sizeof b,"syscall_%ld",n); return b; }

static long cnt[600]; static double tm[600]; static long errs[600];
static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+t.tv_nsec/1e9; }

static void readstr(pid_t p, unsigned long addr, char *out, int max){
  int i=0; while(i<max-1){ long w=ptrace(PTRACE_PEEKDATA,p,addr+i,0); memcpy(out+i,&w,8);
    int z=0; for(int k=0;k<8&&i+k<max-1;k++) if(!out[i+k]){z=1;break;} if(z) return; i+=8;} out[max-1]=0; }

int main(int argc,char**argv){
  int summary=0, a=1; if(argc>1&&!strcmp(argv[1],"-c")){summary=1;a=2;}
  if(a>=argc){fprintf(stderr,"usage: %s [-c] prog args\n",argv[0]);return 2;}
  pid_t pid=fork();
  if(!pid){ ptrace(PTRACE_TRACEME,0,0,0); raise(SIGSTOP); execvp(argv[a],argv+a); perror("exec"); _exit(127); }
  int st; waitpid(pid,&st,0);
  ptrace(PTRACE_SETOPTIONS,pid,0,PTRACE_O_TRACESYSGOOD|PTRACE_O_EXITKILL);
  int in=0; double t0=0; long cur=0; struct user_regs_struct r; int status=0;
  for(;;){
    ptrace(PTRACE_SYSCALL,pid,0,0); waitpid(pid,&st,0);
    if(WIFEXITED(st)){status=WEXITSTATUS(st);break;} if(WIFSIGNALED(st)) break;
    if(!(WIFSTOPPED(st)&&WSTOPSIG(st)==(SIGTRAP|0x80))) continue;
    ptrace(PTRACE_GETREGS,pid,0,&r);
    if(!in){ in=1; t0=now(); cur=r.orig_rax;
      if(!summary){ char s[96]=""; fprintf(stderr,"%s(",nm(cur));
        if(cur==257){ readstr(pid,r.rsi,s,96); fprintf(stderr,"AT_FDCWD, \"%s\", 0x%llx",s,r.rdx);}
        else if(cur==0||cur==1) fprintf(stderr,"%lld, ..., %llu",r.rdi,r.rdx);
        else if(cur==3) fprintf(stderr,"%lld",r.rdi);
        else if(cur==231) fprintf(stderr,"%lld",r.rdi);
        else fprintf(stderr,"...");
        fprintf(stderr,")"); }
    } else { in=0; double d=now()-t0; if(cur>=0&&cur<600){cnt[cur]++;tm[cur]+=d; if((long)r.rax<0&&(long)r.rax>-4096)errs[cur]++;}
      if(!summary) fprintf(stderr," = %ld\n",(long)r.rax); }
  }
  if(summary){ long tc=0,te=0; double tt=0; for(int i=0;i<600;i++){tc+=cnt[i];tt+=tm[i];te+=errs[i];}
    fprintf(stderr,"%% time     seconds  usecs/call     calls    errors syscall\n------ ----------- ----------- --------- --------- ----------------\n");
    for(;;){ int best=-1; for(int i=0;i<600;i++) if(cnt[i]&&(best<0||tm[i]>tm[best])) best=i; if(best<0)break;
      fprintf(stderr,"%6.2f %11.6f %11.0f %9ld %9s %s\n", tt>0?100*tm[best]/tt:0, tm[best], 1e6*tm[best]/cnt[best], cnt[best], "", nm(best));
      cnt[best]=0; }
    fprintf(stderr,"------ ----------- ----------- --------- --------- ----------------\n%6.2f %11.6f %11s %9ld %9ld total\n",100.0,tt,"",tc,te); }
  return status; }
