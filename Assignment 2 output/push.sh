#!/bin/bash
# usage: ./push.sh <github-username> <RegisterNumber>
# 1) create an EMPTY repo named CSA0405-<RegisterNumber>-Assignment2 on github.com (public, or private + add faculty)
git remote add origin https://github.com/$1/CSA0405-$2-Assignment2.git
git branch -M main
git push -u origin main
