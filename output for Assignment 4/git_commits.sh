#!/bin/bash
# Run once inside the repo folder to create one progressive commit per task (edit user.name/email first)
git init -q; git add README.md; git commit -qm "Add README"
for t in Task01 Task02 Task03 Task04 Task05 Task06; do git add $t; git commit -qm "$t: solution, code and outputs"; done
git add screenshots; git commit -qm "Add output screenshots and figures"
echo "now: git remote add origin <your github url> && git push -u origin master"
