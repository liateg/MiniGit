#include "commit.hpp"
#include "add.hpp"       // for IndexEntry and index parsing
#include "hash.hpp"      // for computeSHA1()
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
    // TODO: implement
       auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    string ts = ctime(&now_c); 
ts.pop_back(); // remove newline
return ts;

}





// Get the parent commit hash from the current branch
string getParentHash(const string& cbranch) {
    ifstream branch(".minigit/refs/heads/" + cbranch);
    if (!branch) {
        cerr << "Error: Could not open .minigit/refs/heads/" << cbranch << "\n";
        return string(40, '0');
    }

    string line;
    getline(branch, line); 
    branch.close();

    if (line.empty()) {
        return string(40, '0');  // 40-char zero string all 0, the hash is 40 chars so when its empty its 40 os
    } else {
        return line;
    }
}


// Create a new commit directory 
void createCommitObject(const string& commitHash) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath);
    if (!commitFile) {
        cerr << "Error: Could not create commit object file.\n";
    }
    commitFile.close();  
}


// Write metadata (message, timestamp, parent) to commit object
void writeCommitMetadata(const string& commitHash, const string& message,
                         const string& parentHash, const string& timestamp) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath, ios::app);  // Append mode
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



// Write snapshot of filenames and hashes
void writeCommitSnapshot(const string& commitHash,
                         const unordered_map<string, IndexEntry>& indexMap) {
    fs::path commitPath = ".minigit/objects/" + commitHash;
    ofstream commitFile(commitPath, ios::app);  // Append to existing metadata
    if (!commitFile) {
        cerr << "Error: Could not open commit file to write snapshot.\n";
        return;
    }

    for (const auto& [filename, entry] : indexMap) {
        if (!entry.stagedForRemoval) {
            commitFile << filename << " " << entry.currentHash << "\n";
        }
    }

    commitFile.close();
}


// Update branch reference to point to new commit
void updateBranchRef(const std::string& commitHash) {
    string branch = getCurrentBranch();
    ofstream branchFile(".minigit/refs/heads/" + branch);
    if (!branchFile) {
        cerr << "Error: Could not update branch pointer.\n";
        return;
    }
    branchFile << commitHash;
    branchFile.close();
}


// High-level commit function
void createCommit(const std::string& message,
                  const std::unordered_map<std::string, IndexEntry>& indexMap) {
    string timestamp = getCurrentTimestamp();
    string branch = getCurrentBranch();
    string parentHash = getParentHash(branch);

    // Create unique hash for commit
    string preHashData = message + timestamp + parentHash + branch;
    string commitHash = computeHash(preHashData);

    createCommitObject(commitHash);
    writeCommitMetadata(commitHash, message, parentHash, timestamp);
    writeCommitSnapshot(commitHash, indexMap);
    updateBranchRef(commitHash);

    cout << "Commit created successfully with hash: " << commitHash << endl;
}

