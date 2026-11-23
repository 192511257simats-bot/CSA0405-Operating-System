#!/usr/bin/env bash
# Summarise a web-server access log: requests per status code and the busiest client.
set -euo pipefail
log=$(mktemp)
cat > "$log" <<'LOG'
10.0.0.5 - - [08/Oct/2026:10:01:02] "GET /index.html" 200 512
10.0.0.7 - - [08/Oct/2026:10:01:05] "GET /login" 200 1024
10.0.0.5 - - [08/Oct/2026:10:01:09] "POST /login" 302 0
10.0.0.9 - - [08/Oct/2026:10:02:11] "GET /admin" 403 128
10.0.0.5 - - [08/Oct/2026:10:02:15] "GET /report.pdf" 200 40960
10.0.0.7 - - [08/Oct/2026:10:03:01] "GET /missing" 404 64
10.0.0.5 - - [08/Oct/2026:10:03:30] "GET /index.html" 200 512
LOG
echo "Requests per status code:"
# TODO 1 (done): status code is field 7; count each code, sorted by code
awk '{print $7}' "$log" | sort | uniq -c | awk '{print $2, $1}'
echo "Busiest client:"
# TODO 2 (done): IP is field 1; the most frequent one with its count
awk '{print $1}' "$log" | sort | uniq -c | sort -nr | head -1 | awk '{print $2, $1}'
echo "Total bytes sent: $(awk '{s += $8} END {print s}' "$log")"
rm -f "$log"
