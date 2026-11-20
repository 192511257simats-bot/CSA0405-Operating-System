echo "=== startup time (1000 runs each) ==="
for b in bank_static bank_dyn; do
  s=$(date +%s.%N); for i in $(seq 1000); do ./$b >/dev/null; done; e=$(date +%s.%N)
  echo "$b: $(echo "($e-$s)*1000/1000" | bc -l | cut -c1-6) ms per run"; done
echo "=== peak RSS (/usr/bin/time -v) ==="
for b in bank_static bank_dyn; do /usr/bin/time -v ./$b 2>&1 >/dev/null | egrep "Maximum resident|Minor"; done
echo "=== replace library without recompiling ==="
./bank_dyn
cat > encrypt_v2.c <<'EOT'
#include <string.h>
int encrypt_buf(char *buf, int n, int key){ for(int i=0;i<n;i++) buf[i]=(char)((buf[i]^key)+1); return n; }
const char *enc_version(void){ return "ENC v2 (SECURITY FIX)"; }
EOT
cp libenc.so libenc_v1.so
gcc -O2 -fPIC -shared encrypt_v2.c -o libenc.so
./bank_dyn; ./bank_static
cp libenc_v1.so libenc.so
echo "=== missing library ==="
mv libenc.so libenc.hidden
./bank_dyn; echo "exit code: $?"
./dl_demo; echo "exit code: $?"
mv libenc.hidden libenc.so
echo "=== shared pages between two copies ==="
cat > share.c <<'EOT'
#include <stdio.h>
#include <string.h>
#include <unistd.h>
int main(void){ sleep(3); return 0; }
EOT
gcc share.c -o share_dyn
for i in 1 2; do ./share_dyn & done; sleep 1
for p in $(pgrep share_dyn); do echo "PID $p:"; awk '/libc.so.6/ {f=1} /^[0-9a-f]+-/ {if($0!~/libc.so.6/)f=0} f&&/^(Rss|Pss|Shared_Clean|Private_Clean)/' /proc/$p/smaps | awk '{a[$1]+=$2} END{for(k in a) print k, a[k], "kB"}'; done
wait
