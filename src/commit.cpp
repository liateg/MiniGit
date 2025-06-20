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

// Get current timestamp as string (without newline)
string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    string ts = ctime(&now_c);
    if (!ts.empty() && ts.back() == '\n') {
        ts.pop_back();
    }
    return ts;
}

// Create an empty commit object file
void createCommitObject(const string& commitHash) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath);
    if (!commitFile) {
        cerr << "Error: Could not create commit object file.\n";
    }
}

// Write commit metadata (commit hash, parent, timestamp, message)
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
}

// Write the snapshot of staged files (filename + blob hash) to the commit object
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
}

// Update current branch ref to point to the new commit hash
void updateBranchRef(const string& commitHash) {
    string branch = getCurrentBranch();
    ofstream branchFile(".minigit/refs/heads/" + branch);
    if (!branchFile) {
        cerr << "Error: Could not update branch pointer.\n";
        return;
    }
    branchFile << commitHash;
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

    // Build snapshot & detect changes
    for (auto& [filename, entry] : indexMap) {
        if (entry.stagedForRemoval) {
            hasRealChanges = true;
            continue; // skip deleted files
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

            // Save blob if doesn't exist
            fs::path blobPath = ".minigit/objects/" + blobHash;
            if (!fs::exists(blobPath)) {
                ofstream out(blobPath);
                out << content;
            }
        }

        totalContent += filename + content;
    }

    // Prevent empty commit
    if (!hasRealChanges) {
        cout << "No changes to commit.\n";
        return;
    }

    // Generate commit hash
    totalContent += message + timestamp + parentHash + to_string(chrono::system_clock::now().time_since_epoch().count());
    string commitHash = computeHash(totalContent);

    // Create commit object and write metadata
    createCommitObject(commitHash);
    writeCommitMetadata(commitHash, message, parentHash, timestamp);

    // Write snapshot references
    writeCommitSnapshot(commitHash, indexMap);

    // Update branch ref and write index
    updateBranchRef(commitHash);
    writeIndex(indexMap);

    cout << "✅ Commit created: " << commitHash << "\n";
}
