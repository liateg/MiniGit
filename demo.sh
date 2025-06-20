#!/bin/bash

# Reset workspace
rm -rf .minigit
rm -f file*.txt
echo "🔁 Workspace reset."

# --- 1. init ---
./build/minigit init
echo "✅ Initialized repository"

# --- 2. add + commit initial files ---
cp test/file1.txt .
cp test/file2.txt .
cp test/file_conflict.txt .

./build/minigit add file1.txt
./build/minigit add file2.txt
./build/minigit add file_conflict.txt
./build/minigit commit -m "Initial commit"
cp .minigit/refs/heads/main commit1.txt
echo "✅ Committed initial files"

# --- 3. branch ---
./build/minigit branch feature
echo "✅ Created branch 'feature'"

# --- 4. checkout feature ---
./build/minigit checkout feature
echo "✅ Switched to branch 'feature'"

# Modify file1.txt
cp test/file1_feature.txt file1.txt
./build/minigit add file1.txt
./build/minigit commit -m "Feature: updated file1"
cp .minigit/refs/heads/feature commit2.txt
echo "✅ Committed changes to feature branch"

# --- 5. checkout back to main ---
./build/minigit checkout main
echo "✅ Switched back to main branch"

# Modify file_conflict.txt to create a conflict
cp test/file_conflict_main.txt file_conflict.txt
./build/minigit add file_conflict.txt
./build/minigit commit -m "Main: changed conflict file"
cp .minigit/refs/heads/main commit3.txt
echo "✅ Committed conflicting change on main"

# --- 6. merge ---
./build/minigit merge feature
echo "✅ Merged 'feature' into 'main' (check for conflicts)"

# --- 7. diff ---
echo "🧪 Running diff between first and second commit"
commit1=$(cat commit1.txt)
commit2=$(cat commit2.txt)
./build/minigit diff $commit1 $commit2

# --- 8. log ---
echo "🧾 Full Commit Log:"
./build/minigit log
