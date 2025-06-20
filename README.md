# 🧠 MiniGit - A Lightweight Git-like Version Control System

MiniGit is a simplified, educational version control system implemented in C++. It replicates core Git functionalities including adding files, committing changes, viewing logs, branching, checking out commits/branches, merging, and file difference comparison — all without relying on Git internally.

---

## 📦 Features & Commands

### 1. 🔧 `init`
Initialize a new MiniGit repository:

```bash
./build/minigit init
```
Creates the `.minigit/` directory structure:
```
.minigit/
├── HEAD
├── index
├── objects/
└── refs/
    └── heads/
        └── main
```

---

### 2. ➕ `add <filename>`
Stage a file for commit. Computes a blob hash and stores the file if changed:

```bash
./build/minigit add file.txt
```

Supports:
- Change tracking via SHA-1 hashing.
- Stage-for-removal flag for deletions.

---

### 3. ✅ `commit -m "<message>"`
Commit staged files with a message. Saves:
- Commit metadata (timestamp, parent hash).
- Snapshot of file → blob hash mappings.
- File blobs to `.minigit/objects/`.

```bash
./build/minigit commit -m "Initial commit"
```

Prevents empty commits automatically.

---

### 4. 📜 `log`
Show commit history starting from current HEAD:

```bash
./build/minigit log
```

Output:
```
commit: <hash>
timestamp: ...
message: ...
parent: <parent-hash>
```

---

### 5. 🌿 `branch <branch-name>`
Create a new branch from the current HEAD:

```bash
./build/minigit branch dev
```

Creates `.minigit/refs/heads/dev`.

---

### 6. 🔄 `checkout <branch-name>` or `checkout <commit-hash>`
Switch to another branch or restore state of a previous commit:

```bash
./build/minigit checkout dev
./build/minigit checkout abc123def456...
```

- Rewrites working directory to match target.
- Updates `.minigit/HEAD`.

---

### 7. 🔀 `merge <branch-name>`
Merge a target branch into the current branch using a 3-way merge algorithm.

```bash
./build/minigit merge dev
```

- Finds Lowest Common Ancestor (LCA).
- Applies changes from current, target, and LCA snapshots.
- Detects and flags conflicts:
  ```
  ⚠️ CONFLICT: both modified file.txt
  ```

---

### 8. 📂 `diff <commit1> <commit2>`
Compare snapshots between two commits:

```bash
./build/minigit diff abc123 def456
```

Shows line-level changes:
- Lines added/removed.
- Changed content between blob versions.

---

## 🔁 Starting Over

To reset your MiniGit workspace, delete the `.minigit/` directory:

```bash
rm -rf .minigit
```

This clears all commits, branches, and blobs. Useful for clean restarts.

---

## 🛠 Technologies Used

- C++17
- `std::filesystem` for file I/O
- `openssl` for SHA-1 hashing (`-lssl -lcrypto`)
- Command-line interface (CLI)

---

## 📄 License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).


---