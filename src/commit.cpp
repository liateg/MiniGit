#include <exception>
#include "commit.hpp"
#include "add.hpp"       // for IndexEntry and index parsing
#include "hash.hpp"      // for computeHash()
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;
using namespace std;

// Get the current timestamp
string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    string ts = ctime(&now_c); 
    ts.pop_back(); // remove newline
    return ts;
}

// Create a new commit file in objects directory
void createCommitObject(const string& commitHash) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath);
    if (!commitFile) {
        cerr << "Error: Could not create commit object file.\n";
    }
    commitFile.close();
}

// Write metadata to commit object
void writeCommitMetadata(const string& commitHash, const string& message,
                         const string& parentHash, const string& timestamp) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath, ios::app);
    if (!commitFile) {
        cerr << "Error: Could not open commit file for writing.\n";
        return;
    }

    commitFile << "commit: " << commitHash << "\n";
    commitFile << "parent: " << parentHash << "\n";
    commitFile << "timestamp: " << timestamp << "\n";
    commitFile << "message: " << message << "\n";
    commitFile << "blobs:\n";
    commitFile.close();
}

// Write snapshot of filenames and blob hashes
void writeCommitSnapshot(const string& commitHash,
                         const unordered_map<string, IndexEntry>& indexMap) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath, ios::app);
    if (!commitFile) {
        cerr << "Error: Could not open commit file to write snapshot.\n";
        return;
    }

    for (const auto& [filename, entry] : indexMap) {
        if (!entry.stagedForRemoval) {
            commitFile << filename << " " << entry.lastCommitHash << "\n";
        }
    }

    commitFile.close();
}

// Update HEAD to point to latest commit
void updateBranchRef(const string& commitHash) {
    string branch = getCurrentBranch();
    ofstream branchFile(".minigit/refs/heads/" + branch);
    if (!branchFile) {
        cerr << "Error: Could not update branch pointer.\n";
        return;
    }
    branchFile << commitHash;
    branchFile.close();
}

// Main commit creation logic
void createcommit(const string& message,
                  unordered_map<string, IndexEntry>& indexMap) {
    string timestamp = getCurrentTimestamp();
    string branch = getCurrentBranch();
    string parentHash = getParentHash(branch);

    unordered_map<string, string> snapshot;  // filename → blob hash
    string totalContent;
    bool hasRealChanges = false;

    // 1. Build snapshot and check for actual changes
    for (auto& [filename, entry] : indexMap) {
        if (entry.stagedForRemoval) {
            hasRealChanges = true;
            continue;
        }

        ifstream file(filename);
        if (!file) {
            cerr << "Error: Could not open file '" << filename << "'\n";
            continue;
        }

        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        string blobHash = computeHash(content);
        snapshot[filename] = blobHash;

        if (blobHash != entry.lastCommitHash) {
            hasRealChanges = true;
            entry.lastCommitHash = blobHash;

            // Save blob if not already exists
            fs::path blobPath = ".minigit/objects/" + blobHash;
            if (!fs::exists(blobPath)) {
                ofstream out(blobPath);
                out << content;
            }
        }

        totalContent += filename + content;
    }

    // 2. Prevent empty commit
    if (!hasRealChanges) {
        cout << "No changes to commit.\n";
        return;
    }

    // 3. Generate commit hash
    totalContent += message + timestamp + parentHash + to_string(chrono::system_clock::now().time_since_epoch().count());
    string commitHash = computeHash(totalContent);

    // 4. Create commit object and metadata
    createCommitObject(commitHash);
    writeCommitMetadata(commitHash, message, parentHash, timestamp);

    // 5. Write blob references (snapshot)
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath, ios::app);
    for (const auto& [filename, blobHash] : snapshot) {
        commitFile << filename << " " << blobHash << "\n";
    }
    commitFile.close();

    // 6. Update HEAD and write index
    updateBranchRef(commitHash);
    writeIndex(indexMap);

    cout << "✅ Commit created: " << commitHash << "\n";
}
