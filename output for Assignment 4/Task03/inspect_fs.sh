#!/bin/bash
# Task 3.4 - inspect a real file system (run on Linux; uses a 64 MB ext4 image so no root is needed)
set -e
IMG=/tmp/csa0405_ext4.img
truncate -s 64M "$IMG"
mkfs.ext4 -F -q -b 4096 -L CSA0405 "$IMG"
echo '$ ls -i /etc/hostname /etc/passwd /etc/os-release'; ls -i /etc/hostname /etc/passwd /etc/os-release
echo; echo '$ stat /etc/passwd'; stat /etc/passwd
echo; echo '$ df -i /'; df -i / | head -3
echo; echo '$ df -hT /'; df -hT / | head -3
echo; echo '$ tune2fs -l '"$IMG"' | egrep "Filesystem features|Inode count|Free inodes|Block count|Free blocks|Block size|Inode size|Journal|Default mount"'
tune2fs -l "$IMG" | egrep "Filesystem features|Inode count|Free inodes|Block count|Free blocks|Block size|Inode size|Journal|Default mount"
echo; echo '$ dumpe2fs -h '"$IMG"' | head -30'; dumpe2fs -h "$IMG" 2>/dev/null | head -30
