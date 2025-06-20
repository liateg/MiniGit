# MiniGit - A Lightweight Git-like Version Control System

MiniGit is a simple, easy-to-understand version control system written in C++. It mimics the core features of Git—like adding files, committing changes, viewing history, branching, checking out commits or branches, merging, and showing diffs—all without using Git under the hood.

---

## Table of Contents

- [Features & Commands](#features--commands)
- [Installation](#installation)
- [Usage](#usage)
- [How it Works](#how-it-works)
- [Known Issues / Limitations](#known-issues--limitations)
- [Contributing](#contributing)
- [Acknowledgments](#acknowledgments)
- [License](#license)

---

## Features & Commands

### 1.  `init`  
Start fresh by initializing a new MiniGit repository:

```bash
./build/minigit init
````

This creates the `.minigit/` folder structure where everything will be stored:

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

### 2. `add <filename>`

Stage files to be included in your next commit. MiniGit tracks changes using SHA-1 hashes and stores the file if it’s new or changed:

```bash
./build/minigit add file.txt
```

Supports tracking new files, changes, and even removals.

---

### 3. `commit -m "<message>"`

Commit your staged changes with a descriptive message. MiniGit saves the commit metadata (timestamp, parent commit), a snapshot of all tracked files, and the actual file contents.

```bash
./build/minigit commit -m "Initial commit"
```

It won’t let you make empty commits—so only real changes count.

---

### 4. `log`

Want to see your project’s history? This command lists commits starting from the current HEAD:

```bash
./build/minigit log
```

Example output:

```
commit: <hash>
timestamp: ...
message: ...
parent: <parent-hash>
```

---

### 5. `branch <branch-name>`

Create a new branch based on where you currently are:

```bash
./build/minigit branch dev
```

This creates a new branch reference at `.minigit/refs/heads/dev`.

---

### 6. `checkout <branch-name>` or `checkout <commit-hash>`

Switch between branches or jump to a specific commit:

```bash
./build/minigit checkout dev
./build/minigit checkout abc123def456
```

This updates your working directory and points HEAD to the selected branch or commit.

---

### 7. `merge <branch-name>`

Merge another branch into your current one using a three-way merge:

```bash
./build/minigit merge dev
```

It finds the common ancestor, compares changes, and applies them—flagging conflicts if the same file was changed in both places:

```
CONFLICT: both modified file.txt
```

---

### 8. `diff <commit1> <commit2>`

See what changed between two commits, with line-by-line differences:

```bash
./build/minigit diff abc123 def456
```

Shows added, removed, or modified lines.

---

## Installation

### What You’ll Need

* A C++17 compatible compiler (like `g++` or `clang++`)
* OpenSSL libraries for SHA-1 hashing
* CMake (optional, if you want to build that way)

### How to Build

Clone the repo:

```bash
git clone https://github.com/yourusername/minigit.git
cd minigit
```

Build it:

```bash
mkdir build
cd build
cmake ..
make
```

The executable will be at `./build/minigit`.

### Getting Started

Run MiniGit commands like this:

```bash
./build/minigit init
./build/minigit add file.txt
./build/minigit commit -m "Your message"
```

---

## Usage

Initialize a new repo:

```bash
./build/minigit init
```

Stage files:

```bash
./build/minigit add <filename>
```

Commit staged changes:

```bash
./build/minigit commit -m "Commit message"
```

View commit history:

```bash
./build/minigit log
```

Create branches:

```bash
./build/minigit branch <branch-name>
```

Switch branches or commits:

```bash
./build/minigit checkout <branch-name>
./build/minigit checkout <commit-hash>
```

Merge branches:

```bash
./build/minigit merge <branch-name>
```

See diffs between commits:

```bash
./build/minigit diff <commit1> <commit2>
```

---

## Example Workflow

```bash
./build/minigit init
./build/minigit add file1.txt
./build/minigit commit -m "Add file1"
./build/minigit branch dev
./build/minigit checkout dev
./build/minigit add file2.txt
./build/minigit commit -m "Add file2 in dev branch"
./build/minigit checkout main
./build/minigit merge dev
```

---

## How it Works

MiniGit keeps things simple and educational:

* Commits are snapshots of your files stored as filename → blob hash maps.
* File contents (“blobs”) are saved once using SHA-1 hashes to avoid duplicates.
* The index tracks what’s staged for commit.
* Branches are pointers to commit hashes.
* Merging uses a three-way algorithm and flags conflicts.
* Diff shows line changes between commits.

Under the hood, it uses OpenSSL for hashing and C++17’s filesystem for file operations.

---

## Known Issues / Limitations

* No support for renaming or moving files yet
* Binary diffs aren’t supported
* No remote or network features (it’s local only)
* Merge conflicts have to be resolved manually
* Not optimized for huge repositories

---

## Contributing
1. Fork the repo
2. Create a branch (`git checkout -b feature/YourFeature`)
3. Make your changes
4. Commit with a clear message
5. Push your branch
6. Open a Pull Request

---

## Acknowledgments

* Inspired by Git’s core concepts
* Uses OpenSSL for hashing
* Built with C++17 features

---

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).

```
