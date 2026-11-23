#!/usr/bin/env bash
# Disk-space alert: warn if any real (block-device) filesystem is at or above THRESHOLD percent.
THRESHOLD=${1:-80}
df -P | awk -v t="$THRESHOLD" 'NR>1 && $1 ~ /^\/dev\// {gsub("%","",$5); if ($5+0 >= t) printf "ALERT: %s is %s%% full (mounted on %s)\n",$1,$5,$6; else printf "OK: %s is %s%% full\n",$1,$5}'
