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
void createCommit(const string& message,
                  const unordered_map<string, IndexEntry>& indexMap) {
    string timestamp = getCurrentTimestamp();
    string branch = getCurrentBranch();
    string parentHash = getParentHash(branch);

    // Ensure commit hash includes file content + metadata + randomness
    string totalContent;
    for (const auto& [filename, entry] : indexMap) {
        if (!entry.stagedForRemoval) {
            ifstream file(filename);
            string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            totalContent += filename + content;
        }
    }

    // Add commit message, time, parent, and randomness
    totalContent += message + timestamp + parentHash;
    totalContent += to_string(chrono::system_clock::now().time_since_epoch().count());  // Randomized salt

    // Final commit hash
    string commitHash = computeHash(totalContent);

    // 🔧 Create the commit object
    createCommitObject(commitHash);
    writeCommitMetadata(commitHash, message, parentHash, timestamp);
    writeCommitSnapshot(commitHash, indexMap);
    updateBranchRef(commitHash);

    cout << "Commit created successfully with hash: " << commitHash <<endl;
    
}
