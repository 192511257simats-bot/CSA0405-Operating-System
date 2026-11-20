set -x
gcc -std=c11 -Wall -Wextra -O2 -fPIC -shared encrypt.c -o libenc.so
gcc -std=c11 -Wall -Wextra -O2 bank.c -L. -lenc -Wl,-rpath,'$ORIGIN' -o bank_dyn
gcc -std=c11 -Wall -Wextra -O2 -static bank.c encrypt.c -o bank_static
gcc -std=c11 -Wall -Wextra -O2 dl.c -ldl -o dl_demo
ls -l bank_dyn bank_static libenc.so
ldd bank_dyn
ldd bank_static
./bank_dyn; ./bank_static; ./dl_demo
echo "--- maps (dynamic)"; ./bank_dyn m
echo "--- maps (static)"; ./bank_static m
